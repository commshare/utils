// https://stackoverflow.com/questions/11711920/how-to-implement-multithread-safe-singleton-in-c11-without-using-mutex
// Created by zhangbin on 2019/11/24.
//

#ifndef LICODE_EJSINGLETON_H
#define LICODE_EJSINGLETON_H
#include <memory>
#include <mutex>
namespace EgdCore
{
template<typename T>
class Singleton
{
private:
  Singleton<T>(const Singleton<T>&) = delete;
  Singleton<T>& operator = (const Singleton<T>&) = delete;

  Singleton<T>() = default;

  static std::unique_ptr<T> m_instance;
  static std::once_flag m_once;

public:
  virtual ~Singleton<T>() = default;

  static T* getInstance()
  {
    std::call_once(m_once, []() {
      m_instance.reset(new T);
    });
    return m_instance.get();
  }

  template<typename... Args>
  static T* getInstance2nd(Args&& ...args)
  {
    std::call_once(m_once, [&]() {
      m_instance.reset(new T(std::forward<Args>(args)...));
    });
    return m_instance.get();
  }
};

template<typename T> std::unique_ptr<T> Singleton<T>::m_instance;
template<typename T> std::once_flag Singleton<T>::m_once;
}



#endif //LICODE_EJSINGLETON_H
