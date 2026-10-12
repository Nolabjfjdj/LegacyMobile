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
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

using DWORD = std::uint32_t;

namespace ios_port_tls {
inline std::atomic<DWORD> nextIndex{1};
inline thread_local std::unordered_map<DWORD, void*> values;
}

inline DWORD TlsAlloc() {
    return ios_port_tls::nextIndex.fetch_add(1, std::memory_order_relaxed);
}

inline int TlsSetValue(DWORD index, void* value) {
    ios_port_tls::values[index] = value;
    return 1;
}

inline void* TlsGetValue(DWORD index) {
    const auto it = ios_port_tls::values.find(index);
    return it == ios_port_tls::values.end() ? nullptr : it->second;
}
