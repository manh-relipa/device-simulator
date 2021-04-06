#ifndef SINGLETON_H
#define SINGLETON_H
#include <memory>

template <typename T>
std::shared_ptr<T> getInstance() {
    static std::shared_ptr<T> instance;
    if (instance == nullptr) {
        instance = std::shared_ptr<T>(new T);
    }
    return instance;
}

#endif // SINGLETON_H
