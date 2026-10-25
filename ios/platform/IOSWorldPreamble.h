#pragma once

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <exception>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

using DWORD = std::uint32_t;
using __int64 = long long;
using byte = unsigned char;

struct CRITICAL_SECTION {
    std::recursive_mutex mutex;
};

inline void InitializeCriticalSection(CRITICAL_SECTION*) {}
inline void DeleteCriticalSection(CRITICAL_SECTION*) {}
inline void EnterCriticalSection(CRITICAL_SECTION* section) {
    section->mutex.lock();
}
inline void LeaveCriticalSection(CRITICAL_SECTION* section) {
    section->mutex.unlock();
}

namespace ios_port_tls {
inline std::atomic<DWORD>& nextIndex() {
    static std::atomic<DWORD> index{1};
    return index;
}

inline std::unordered_map<DWORD, void*>& values() {
    static thread_local std::unordered_map<DWORD, void*> storage;
    return storage;
}
}

inline DWORD TlsAlloc() {
    return ios_port_tls::nextIndex().fetch_add(1, std::memory_order_relaxed);
}

inline int TlsSetValue(DWORD index, void* value) {
    ios_port_tls::values()[index] = value;
    return 1;
}

inline void* TlsGetValue(DWORD index) {
    const auto& values = ios_port_tls::values();
    const auto it = values.find(index);
    return it == values.end() ? nullptr : it->second;
}
