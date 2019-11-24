//Copyright (c) 2017 Finjin
//
//This file is part of Finjin Common (finjin-common).
//
//Finjin Common is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//This Source Code Form is subject to the terms of the Mozilla Public
//License, v. 2.0. If a copy of the MPL was not distributed with this
//file, You can obtain one at http://mozilla.org/MPL/2.0/.


#pragma once

//          Copyright Oliver Kowalke 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


//Includes----------------------------------------------------------------------
#include "finjin/common/FiberException.hpp"
#include "finjin/common/JobSharedState.hpp"
#include "JobSharedStateObject.hpp"
#include "finjin/common/JobFuture.hpp"
#include <utility>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename R>
    class promise
    {
    public:
        promise(const promise&) = delete;
        promise& operator = (const promise&) = delete;

    public:
        promise() : obtained_(false), future_()
        {
            typedef detail::shared_state_object<R, std::allocator<promise<R> > > object_t;

            std::allocator<promise<R> > alloc;
            typename object_t::allocator_t a(alloc);
            future_ = ptr_t(::new(a.allocate(1)) object_t(a));
        }

        template <typename Allocator>
        promise(std::allocator_arg_t, Allocator alloc) : obtained_(false), future_()
        {
            typedef detail::shared_state_object<R, Allocator> object_t;

            typename object_t::allocator_t a(alloc);
            future_ = ptr_t(::new(a.allocate(1)) object_t(a));
        }

        ~promise()
        {
            if (future_)
                future_->owner_destroyed();
        }

        promise(promise&& other) noexcept : obtained_(other.obtained_), future_(std::move(other.future_))
        {
            other.obtained_ = false;
        }

        promise& operator = (promise&& other) noexcept
        {
            if (this != &other)
            {
                obtained_ = other.obtained_;

                other.obtained_ = false;

                future_ = std::move(other.future_);
            }
            return *this;
        }

        void swap(promise& other) noexcept
        {
            std::swap(obtained_, other.obtained_);
            future_.swap(other.future_);
        }

        future<R> get_future()
        {
            if (obtained_)
                throw future_already_retrieved();

            if (!future_)
                throw promise_uninitialized();

            obtained_ = true;

            return future<R>(future_);
        }

        void set_value(const R& value)
        {
            if (!future_)
                throw promise_uninitialized();

            future_->set_value(value);
        }

        void set_value(R&& value)
        {
            if (!future_)
                throw promise_uninitialized();

            future_->set_value(std::move(value));
        }

        void set_exception(std::exception_ptr p)
        {
            if (!future_)
                throw promise_uninitialized();

            future_->set_exception(p);
        }

    private:
        typedef typename detail::shared_state<R>::ptr_t ptr_t;

        bool obtained_;
        ptr_t future_;
    };

    template <typename R>
    class promise<R&>
    {
    public:
        promise(const promise&) = delete;
        promise& operator = (const promise&) = delete;

    public:
        promise() : obtained_(false), future_()
        {
            typedef detail::shared_state_object<R&, std::allocator<promise<R&> > > object_t;

            std::allocator<promise<R> > alloc;
            typename object_t::allocator_t a(alloc);
            future_ = ptr_t(::new(a.allocate(1)) object_t(a));
        }

        template <typename Allocator>
        promise(std::allocator_arg_t, Allocator alloc) : obtained_(false), future_()
        {
            typedef detail::shared_state_object<R&, Allocator> object_t;

            typename object_t::allocator_t a(alloc);
            future_ = ptr_t(::new(a.allocate(1)) object_t(a));
        }

        promise(promise&& other) noexcept : obtained_(other.obtained_), future_(std::move(other.future_))
        {
            other.obtained_ = false;
        }

        ~promise()
        {
            if (future_)
                future_->owner_destroyed();
        }

        promise& operator = (promise&& other) noexcept
        {
            if (this != & other)
            {
                obtained_ = other.obtained_;

                other.obtained_ = false;

                future_ = std::move(other.future_);
            }
            return *this;
        }

        void swap(promise& other) noexcept
        {
            std::swap(obtained_, other.obtained_);
            future_.swap(other.future_);
        }

        future<R&> get_future()
        {
            if (obtained_)
                throw future_already_retrieved();

            if (!future_)
                throw promise_uninitialized();

            obtained_ = true;

            return future<R&>(future_);
        }

        void set_value(R& value)
        {
            if (!future_)
                throw promise_uninitialized();

            future_->set_value(value);
        }

        void set_exception(std::exception_ptr p)
        {
            if (!future_)
                throw promise_uninitialized();

            future_->set_exception(p);
        }

    private:
        typedef typename detail::shared_state<R& >::ptr_t ptr_t;

        bool obtained_;
        ptr_t future_;
    };

    template <>
    class promise<void>
    {
    public:
        promise(const promise&) = delete;
        promise& operator = (const promise&) = delete;

    public:
        promise() : obtained_(false), future_()
        {
            typedef detail::shared_state_object<void, std::allocator<promise<void>> > object_t;

            std::allocator<promise<void>> alloc;
            object_t::allocator_t a(alloc);
            future_ = ptr_t(::new(a.allocate(1)) object_t(a));
        }

        template <typename Allocator>
        promise(std::allocator_arg_t, Allocator alloc) : obtained_(false), future_()
        {
            typedef detail::shared_state_object<void, Allocator>  object_t;

            typename object_t::allocator_t a(alloc);
            future_ = ptr_t(::new(a.allocate(1)) object_t(a));
        }

        ~promise()
        {
            if (future_)
                future_->owner_destroyed();
        }

        inline promise(promise&& other) noexcept : obtained_(other.obtained_), future_(std::move(other.future_))
        {
            other.obtained_ = false;
        }

        inline promise& operator = (promise&& other) noexcept
        {
            if (this != & other)
            {
                obtained_ = other.obtained_;

                other.obtained_ = false;

                future_ = std::move(other.future_);
            }
            return *this;
        }

        inline void swap(promise& other) noexcept
        {
            std::swap(obtained_, other.obtained_);
            future_.swap(other.future_);
        }

        inline future<void> get_future()
        {
            if (obtained_)
                throw future_already_retrieved();

            if (!future_)
                throw promise_uninitialized();

            obtained_ = true;

            return future<void>(future_);
        }

        inline void set_value()
        {
            if (!future_)
                throw promise_uninitialized();

            future_->set_value();
        }

        inline void set_exception(std::exception_ptr p)
        {
            if (!future_)
                throw promise_uninitialized();

            future_->set_exception(p);
        }

    private:
        typedef detail::shared_state<void>::ptr_t ptr_t;

        bool obtained_;
        ptr_t future_;
    };

    template <typename R>
    void swap(promise<R>& l, promise<R>& r)
    {
        l.swap(r);
    }

} }
