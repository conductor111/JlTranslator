#pragma once

#include <memory>

template<typename T>
class pimpl
{
public:
    template<typename... Arg>
    pimpl(Arg&&... arg) : d{ new T(std::forward<Arg>(arg)...) } {}
protected:
    std::unique_ptr<T> d;
};
