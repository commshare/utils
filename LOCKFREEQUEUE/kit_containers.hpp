/*
 * This is free and unencumbered software released into the public domain. 
 */
#pragma once
#include "kit_config.hpp"
#include "concurrentqueue.h"


namespace kit
{
	struct _LockFreeTraits : public moodycamel::ConcurrentQueueDefaultTraits
	{
		static inline void* malloc(std::size_t size);
		static inline void free(void* ptr);
	};

	/// @brief A typedef of the
	/// [moodycamel::ConcurrentQueue](https://github.com/cameron314/concurrentqueue) which uses the
	///   kit::Allocator.
	/// @note It does not use the kit::Allocator directly, but they are both funnled down to the same
	///   function calls.
	template< typename Type >
	using LockFreeQueue = moodycamel::ConcurrentQueue< Type, _LockFreeTraits >;
} // namespace kit

#include "kit_containers.inl"
