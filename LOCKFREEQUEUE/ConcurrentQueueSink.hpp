#pragma once

#include <concurrentqueue.h>
#include <mutex>

#include "logger/Sink.hpp"
#include "logger/Formatter.hpp"

namespace logger
{
namespace details
{

class ConcurrentQueueSink : public Sink
{
public:
  explicit ConcurrentQueueSink(std::shared_ptr< Sink > aSink)
    :
    internalSink(aSink)
  {
  }

  virtual void send(std::unique_ptr<Message> message) override
  {
    messages.enqueue(std::move(message));
  }

  virtual void flush() override
  {
    std::lock_guard< std::mutex > lock(flushMt);

    std::unique_ptr<Message> message;
    while (messages.try_dequeue(message))
    {
      internalSink->send(std::move(message));
    }

    internalSink->flush();
  }

private:
  std::shared_ptr< Sink > internalSink;

  std::mutex flushMt; // only one thread can be inside flush() method at the time

  struct MyTraits : public moodycamel::ConcurrentQueueDefaultTraits
  {
    static const size_t BLOCK_SIZE = sizeof(std::unique_ptr<Message>);       // Use bigger blocks
  };

  moodycamel::ConcurrentQueue< std::unique_ptr<Message>, MyTraits > messages;
};

} // details
} // logger