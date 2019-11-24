/*
 * Prime utility library
 * Copyright (c) 2013 Jonathan W. Armond
 */
#pragma once

#include <future>
#include <memory>

#include "async_value.hpp"
#include "locking_stack.hpp"

namespace putil {

/** Abstract task type-erasure implementation base class. */
class taskimpl_base {
public:
    typedef std::unique_ptr<taskimpl_base> taskimpl_ptr;

    virtual ~taskimpl_base() {}
//    void* operator new(size_t n)
//    {}
//
//    void operator delete(void* p)
//    {}

    virtual void operator()() =0;
};

/** Concrete derived task type-erasure implementation for function returning a result
    by promise. */
template<typename Result, typename Function>
class taskimpl : public taskimpl_base {
public:
    typedef std::unique_ptr<taskimpl> taskimpl_ptr;

    taskimpl(const Function& func) noexcept :
       func_(func) {}
    virtual void operator()()
    {
        promise_.set_value(func_());
    }

    std::future<Result> get_future() { return promise_.get_future(); }

private:
    std::promise<Result> promise_;
    Function func_;
};

// FIXME can get around the void specialization by using a helper function
// template.

/** Partial specialization for no result. */
template<typename Function>
class taskimpl<void, Function> : public taskimpl_base {
public:
    typedef std::unique_ptr<taskimpl> taskimpl_ptr;

    taskimpl(const Function& func) noexcept :
       func_(func) {}
    virtual void operator()()
    {
        func_();
        promise_.set_value();
    }

    std::future<void> get_future() { return promise_.get_future(); }

private:
    std::promise<void> promise_;
    Function func_;
};

/** Helper to queue task on scheduler. */
void queue_task(std::unique_ptr<taskimpl_base>&& taskPtr);

/** Task object with result. Automatically schedules to execution. */
template<typename Result>
class task {
public:
    task() {}

    task(task&& other) noexcept :
        future_(std::move(other.future_)) {}

    task& operator=(task&& other) noexcept
    {
        future_ = std::move(other.future_);
        return *this;
    }

    template<typename Function>
    task(const Function& func)
    {
        std::unique_ptr<taskimpl<Result,Function>> newTask(
            new taskimpl<Result,Function>(func));
        future_ = newTask->get_future();
        queue_task(std::move(newTask));
    }

    task(const task&) =delete;
    task& operator=(const task&) =delete;

    bool valid() const { return future_.valid(); }
    void wait() const { return future_.wait(); }
    Result get() { return this->future_.get(); }

    putil::async_value<Result> get_async_value()
    {
        return putil::async_value<Result>(std::move(future_));
    }
    
private:
    std::future<Result> future_;
};

/** Partial specialization for reference. */
template<typename Result>
class task<Result&> {
public:
    task() noexcept {}

    task(task&& other) noexcept :
        future_(std::move(other.future_)) {}

    task& operator=(task&& other) noexcept
    {
        future_ = std::move(other.future_);
        return *this;
    }

    template<typename Function>
    task(const Function& func)
    {
        std::unique_ptr<taskimpl<Result,Function>> newTask(
            new taskimpl<Result,Function>(func));
        future_ = newTask->get_future();
        queue_task(std::move(newTask));
    }

    task(const task&) =delete;
    task& operator=(const task&) =delete;

    bool valid() const { return future_.valid(); }
    void wait() const { return future_.wait(); }
    Result& get() { return this->future_.get(); }

private:
    std::future<Result&> future_;
};


/** Partial specialization for void result. */
template<>
class task<void> {
public:
    task() noexcept {}

    task(task&& other) noexcept :
        future_(std::move(other.future_)) {}

    task& operator=(task&& other) noexcept
    {
        future_ = std::move(other.future_);
        return *this;
    }

    template<typename Function>
    task(const Function& func)
    {
        std::unique_ptr<taskimpl<void,Function>> newTask(
            new taskimpl<void,Function>(func));
        future_ = newTask->get_future();
        queue_task(std::move(newTask));
    }

    task(const task&) =delete;
    task& operator=(const task&) =delete;

    bool valid() const { return future_.valid(); }
    void wait() const { return future_.wait(); }
    void get() { return this->future_.get(); }
    
private:
    std::future<void> future_;
};


template<typename Result>
class task_set {
public:
    task_set() noexcept {}

    task_set(size_t reserve_size)
    {
        tasks_.reserve(reserve_size);
    }

    void push(task<Result>&& t)
    {
        tasks_.push(std::move(t));
    }

    void wait()
    {
        task<Result> t;
        while (tasks_.try_pop(t))
            t.get();
    }

    std::vector<Result> get_results()
    {
        std::vector<Result> results;
        results.reserve(tasks_);

        task<Result> t;
        while (tasks_.try_pop(t))
            results.push_back(t.get());

        return results;
    }

private:
    putil::locking_stack<task<Result>> tasks_;
};


}
