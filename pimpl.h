#pragma once

template<typename T>
class pimpl
{
public:
    pimpl() : d(new T) {}
protected:
    std::unique_ptr<T> d;
};