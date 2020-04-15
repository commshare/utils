//https://gist.github.com/zhiguangwang/6192e8304ca61d18c72350456edcf736
#include <iostream>
#include <functional>
#include <chrono>

#define BIND_THIS(MemFn) \
    [this](auto&&... args) { return (this)->MemFn(std::forward<decltype(args)>(args)...); }

#define BIND_REF(ObjectRef, MemFn) \
    [&_r = (ObjectRef)](auto&&... args) { return _r.MemFn(std::forward<decltype(args)>(args)...); }

#define BIND_PTR(ObjectPtr, MemFn) \
    [_p = (ObjectPtr)](auto&&... args) { return _p->MemFn(std::forward<decltype(args)>(args)...); }

namespace cr = std::chrono;

using my_clock = cr::high_resolution_clock;

using Callback = std::function<void()>;

static const int kIterateCount = 100'000'000;

class CallbackHolder
{
public:

    void SetCallbackValue(Callback cb)
    {
        cb_ = std::move(cb);
    }

    void SetCallbackConstRef(const Callback& cb)
    {
        cb_ = cb;
    }

    void SetCallbackRefRef(Callback&& cb)
    {
        cb_ = std::move(cb);
    }

    void DoCallback()
    {
        cb_();
    }

private:

    Callback cb_;
};

class CallbackProvider
{
public:

    CallbackProvider()
        : value_(0)
    {
    }

    void DoSomething()
    {
        ++value_;
    }

    int GetValue() const { return value_; }

private:

    int value_;
};

void TestConstRef()
{
    auto start_time = my_clock::now();

    CallbackHolder holder;
    CallbackProvider provider;
    for (int i = 0; i < kIterateCount; ++i)
    {
        holder.SetCallbackConstRef(BIND_REF(provider, CallbackProvider::DoSomething));
        holder.DoCallback();
    }

    auto end_time = my_clock::now();
    auto diff = end_time - start_time;
    auto milliseconds = cr::duration_cast<cr::milliseconds>(diff);
    auto millisecond_count = milliseconds.count();

    std::cout << "TestConstRef: " << millisecond_count << " ms\n";
}

void TestRefRef()
{
    auto start_time = my_clock::now();

    CallbackHolder holder;
    CallbackProvider provider;
    for (int i = 0; i < kIterateCount; ++i)
    {
        holder.SetCallbackRefRef(BIND_REF(provider, CallbackProvider::DoSomething));
        holder.DoCallback();
    }

    auto end_time = my_clock::now();
    auto diff = end_time - start_time;
    auto milliseconds = cr::duration_cast<cr::milliseconds>(diff);
    auto millisecond_count = milliseconds.count();

    std::cout << "TestRefRef: " << millisecond_count << " ms\n";
}

void TestValue()
{
    auto start_time = my_clock::now();

    CallbackHolder holder;
    CallbackProvider provider;
    for (int i = 0; i < kIterateCount; ++i)
    {
        holder.SetCallbackValue(BIND_REF(provider, CallbackProvider::DoSomething));
        holder.DoCallback();
    }

    auto end_time = my_clock::now();
    auto diff = end_time - start_time;
    auto milliseconds = cr::duration_cast<cr::milliseconds>(diff);
    auto millisecond_count = milliseconds.count();

    std::cout << "TestValue: " << millisecond_count << " ms\n";
}

int main()
{
    TestConstRef();
    TestRefRef();
    TestValue();
}
