#pragma once

#include <string>
#include <stdexcept>

#include <fmt/ostream.h>

namespace mfl {

  /////////////////////////////////////
  // Exception wrapper
  class Exception : public std::runtime_error {
  public:
    explicit Exception(const std::string & message) :
        std::runtime_error(message) {
      fmt::print(stderr, message + "\n");
    }

    template<typename ... Args>
    static Exception build(const char * format, const Args & ... args) {
      return Exception(fmt::format(format, args ...));
    }
  };

}

