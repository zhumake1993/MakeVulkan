#pragma once

#include <unordered_map>

// mkHashMap
template <class _Kty, class _Ty, class _Hasher = std::hash<_Kty>, class _Keyeq = std::equal_to<_Kty>>
using mkHashMap = std::unordered_map<_Kty, _Ty, _Hasher, _Keyeq>;