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
#include <cstdlib>
#include <cstring>
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
using BYTE = unsigned char;
using ULONG_PTR = std::uintptr_t;
constexpr ULONG_PTR MAXULONG_PTR = std::numeric_limits<ULONG_PTR>::max();
constexpr unsigned long PAGE_READWRITE = 0x04;

inline void* XPhysicalAlloc(std::size_t size, ULONG_PTR, ULONG_PTR alignment, unsigned long) {
    void* memory = nullptr;
    const std::size_t safeAlignment = alignment < sizeof(void*) ? sizeof(void*) : alignment;
    return posix_memalign(&memory, safeAlignment, size) == 0 ? memory : nullptr;
}
inline void XPhysicalFree(void* memory) { std::free(memory); }
inline void* XMemCpy(void* destination, const void* source, std::size_t size) { return std::memcpy(destination, source, size); }
inline void* XMemSet(void* destination, int value, std::size_t size) { return std::memset(destination, value, size); }

struct CRITICAL_SECTION {
    std::recursive_mutex mutex;
};

inline void InitializeCriticalSection(CRITICAL_SECTION*) {}
inline int InitializeCriticalSectionAndSpinCount(CRITICAL_SECTION* section, unsigned long) {
    InitializeCriticalSection(section);
    return 1;
}
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
