#include "Singleton.h"

template<typename T>
T& Singleton<T>::instance()
{
    static T instance{};
    return instance;
}