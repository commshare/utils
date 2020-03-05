#pragma once

#include <chrono>
#include <string>

#include <fmt/format.h>

namespace mfl {
  class Timer {
  private:
    std::chrono::high_resolution_clock::time_point mBegin;
    std::chrono::high_resolution_clock::time_point mEnd;

  public:
    Timer() = default;

    inline void start() {
      mBegin = std::chrono::high_resolution_clock::now();
    }

    inline void stop() {
      mEnd = std::chrono::high_resolution_clock::now();
    }

    inline auto getDuration() {
      return
          std::
          chrono::
          duration_cast<std::chrono::milliseconds>(mEnd - mBegin).count();
    }
  };
}
