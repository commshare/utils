// g++-7 sharding.cc -Wall -std=c++17 -g -pthread -O2 -ltbb
//https://gist.github.com/abcdabcd987/53b7aa6fdb8f7dbe46798fa6df2f5871

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <optional>
#include <random>
#include <unordered_map>
#include <thread>
#include <iostream>
#include "tbb/concurrent_hash_map.h"
#include "libcuckoo/cuckoohash_map.hh"

std::string random_string(int len) {
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static std::mt19937 gen(123);
    static std::uniform_int_distribution<int> dist(0, sizeof(alphanum) - 1);
    std::string s;
    for (int i = 0; i < len; ++i)
        s += alphanum[dist(gen)];
    return s;
}

class KVBigLock {
    std::mutex m_mutex;
    std::unordered_map<std::string, int> m_map;

public:
    void put(const std::string &key, int value) {
        std::lock_guard lock(m_mutex);
        m_map.emplace(key, value);
    }

    std::optional<int> get(const std::string &key) {
        std::lock_guard lock(m_mutex);
        auto it = m_map.find(key);
        if (it != m_map.end())
            return it->second;
        return {};
    }

    bool remove(const std::string &key) {
        std::lock_guard lock(m_mutex);
        auto n = m_map.erase(key);
        return n;
    }
};

class KVSharedLock {
    std::shared_mutex m_mutex;
    std::unordered_map<std::string, int> m_map;

public:
    void put(const std::string &key, int value) {
        std::lock_guard lock(m_mutex);
        m_map.emplace(key, value);
    }

    std::optional<int> get(const std::string &key) {
        std::shared_lock lock(m_mutex);
        auto it = m_map.find(key);
        if (it != m_map.end())
            return it->second;
        return {};
    }

    bool remove(const std::string &key) {
        std::lock_guard lock(m_mutex);
        auto n = m_map.erase(key);
        return n;
    }
};

class KVSharded {
    const size_t m_mask;
    std::vector<KVBigLock> m_shards;

    KVBigLock& get_shard(const std::string &key) {
        std::hash<std::string> hash_fn;
        auto h = hash_fn(key);
        return m_shards[h & m_mask];
    }

public:
    KVSharded(size_t num_shard): m_mask(num_shard-1), m_shards(num_shard) {
        if ((num_shard & m_mask) != 0)
            throw std::runtime_error("num_shard must be a power of two");
    }

    void put(const std::string &key, int value) {
        get_shard(key).put(key, value);
    }

    std::optional<int> get(const std::string &key) {
        return get_shard(key).get(key);
    }

    bool remove(const std::string &key) {
        return get_shard(key).remove(key);
    }
};

class KVLibCuckoo {
    cuckoohash_map<std::string, int> m_map;

public:
    void put(const std::string &key, int value) {
        m_map.insert(key, value);
    }

    std::optional<int> get(const std::string &key) {
        try {
            return m_map.find(key);
        } catch (const std::out_of_range& e) {
            return {};
        }
    }

    bool remove(const std::string &key) {
        auto n = m_map.erase(key);
        return n;
    }
};

class KVIntelTBB {
    tbb::concurrent_hash_map<std::string, int> m_map;

public:
    void put(const std::string &key, int value) {
        m_map.emplace(key, value);
    }

    std::optional<int> get(const std::string &key) {
        tbb::concurrent_hash_map<std::string, int>::const_accessor accessor;
        bool found = m_map.find(accessor, key);
        if (!found) return {};
        return accessor->second;
    }

    bool remove(const std::string &key) {
        auto n = m_map.erase(key);
        return n;
    }
};

template<class T>
void do_worker(size_t seed, T& kv, const std::vector<std::string>& key_set, size_t num_ops) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution<int> dist_choice(1, 100), dist_key(0, key_set.size()-1), dist_value;
    for (size_t i = 0; i < num_ops; ++i) {
        auto choice = dist_choice(gen);
        auto &key = key_set[dist_key(gen)];
        if (choice <= 33)
            kv.put(key, dist_value(gen));
        else if (choice <= 66)
            (void) kv.get(key);
        else
            (void) kv.remove(key);
    }
}

template<class T, typename ...Args>
void run(size_t num_workers, size_t num_keys, size_t num_ops, const std::vector<std::string> &key_set, Args&& ...args) {
    for (size_t i = 0; i < 16; ++i) {
        T kv(std::forward<Args>(args)...);

        std::vector<std::thread> workers;
        for (size_t i = 0; i < num_workers; ++i)
            workers.emplace_back(do_worker<T>, i+1, std::ref(kv), std::ref(key_set), num_ops);

        auto st = std::chrono::high_resolution_clock::now();
        for (auto &worker : workers)
            worker.join();
        auto ed = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = ed-st;
        double time = diff.count();

        std::cout << time << ' ' << std::flush;
    }
    std::cout << std::endl;
}

int main(int argc, char** argv) {
    const size_t num_workers = std::stoi(argv[1]);
    const size_t num_keys = 1000000;
    const size_t num_ops = 10000000;

    std::vector<std::string> key_set;
    for (size_t i = 0; i < num_keys; ++i)
        key_set.emplace_back(random_string(32));

    if (strcmp(argv[2], "KVBigLock") == 0)
        run<KVBigLock>(num_workers, num_keys, num_ops, key_set);
    else if (strcmp(argv[2], "KVSharedLock") == 0)
        run<KVSharedLock>(num_workers, num_keys, num_ops, key_set);
    else if (strcmp(argv[2], "KVSharded") == 0)
        run<KVSharded>(num_workers, num_keys, num_ops, key_set, std::stoi(argv[3]));
    else if (strcmp(argv[2], "KVLibCuckoo") == 0)
        run<KVLibCuckoo>(num_workers, num_keys, num_ops, key_set);
    else if (strcmp(argv[2], "KVIntelTBB") == 0)
        run<KVIntelTBB>(num_workers, num_keys, num_ops, key_set);
    else
        return 1;
}
