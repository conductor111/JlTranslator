#pragma once

#include <memory>

template<typename T, typename... A>
class pimpl
{
public:
    pimpl(A... a) : d{new T(a...)} {}
protected:
    std::unique_ptr<T> d;
};
