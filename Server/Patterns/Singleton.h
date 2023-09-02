#ifndef SINGLETON_H
#define SINGLETON_H
template <typename T> class Singleton {
public:
  static T &instance();

protected:
  Singleton() {}
};
#endif // !SINGLETON_H