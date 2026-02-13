#pragma once
#include <cstdlib>   // malloc, free
#include <cstring>   // memcpy
#include <cassert>

template<typename T>
class Array
{
public:
    Array()
        : _data(nullptr), _size(0), _capacity(0) {}

    ~Array()
    {
        clear();
        free(_data);
    }

    // --- Capacity info ---
    size_t size() const     { return _size; }
    size_t capacity() const { return _capacity; }
    T* data() const { return _data; }
    bool empty() const      { return _size == 0; }

    // --- Access ---
    T& operator[](size_t index)
    {
        assert(index < _size);
        return _data[index];
    }

    const T& operator[](size_t index) const
    {
        assert(index < _size);
        return _data[index];
    }

    // --- Modifiers ---
    void push_back(const T& value)
    {
        if (_size >= _capacity)
            reserve(_capacity == 0 ? 4 : _capacity * 2);

        _data[_size++] = value;
    }

    void pop_back()
    {
        assert(_size > 0);
        _size--;
    }

    void clear()
    {
        _size = 0;
    }

    void reserve(size_t newCapacity)
    {
        if (newCapacity <= _capacity)
            return;

        T* new_Data = (T*)malloc(sizeof(T) * newCapacity);
        assert(new_Data);

        if (_data)
        {
            memcpy(new_Data, _data, sizeof(T) * _size);
            free(_data);
        }

        _data = new_Data;
        _capacity = newCapacity;
    }

private:
    T* _data;
    size_t _size;
    size_t _capacity;
};
