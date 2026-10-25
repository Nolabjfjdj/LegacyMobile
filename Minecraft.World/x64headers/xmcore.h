#pragma once

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <mutex>
#include <vector>

using __uint64 = unsigned long long;
using PlayerUID = unsigned long long;

template <class T>
class arrayWithLength
{
public:
    T *data;
    unsigned int length;

    arrayWithLength() : data(nullptr), length(0) {}
    arrayWithLength(unsigned int elements, bool clearArray = true)
        : data(elements ? new T[elements] : nullptr), length(elements)
    {
        if (clearArray && data) {
            std::memset(data, 0, sizeof(T) * elements);
        }
    }
    arrayWithLength(T *source, unsigned int elements) : data(source), length(elements) {}
    T& operator[](unsigned int index) { return data[index]; }
    T operator[](unsigned int index) const { return data[index]; }
};

using byteArray = arrayWithLength<byte>;
using charArray = arrayWithLength<char>;

#include "DataInputStream.h"
#include "DataOutputStream.h"

template <typename T>
class XLockFreeStack
{
    std::vector<T *> entries;
    std::mutex mutex;

public:
    void Initialize() {}
    void Push(T *value)
    {
        std::lock_guard<std::mutex> lock(mutex);
        entries.push_back(value);
    }
    T *Pop()
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (entries.empty()) {
            return nullptr;
        }
        T *value = entries.back();
        entries.pop_back();
        return value;
    }
};
