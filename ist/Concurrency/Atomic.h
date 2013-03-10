﻿#ifndef ist_Concurrency_Atomic_h
#define ist_Concurrency_Atomic_h

#include "../Config.h"

namespace ist {

#if defined(ist_env_Windows)
class istInterModule atomic_int32
{
public:
    atomic_int32() : m_value(0) {}
    atomic_int32(int32 v) : m_value(v) {}

    int32 swap(int32 v) { return _InterlockedExchange(&m_value, v); }
    int32 compare_and_swap(int32 v, int32 comp) { return _InterlockedCompareExchange(&m_value, v, comp); }
    int32 operator+=(int32 v)   { return _InterlockedExchangeAdd(&m_value, v); }
    int32 operator-=(int32 v)   { return _InterlockedExchangeAdd(&m_value,-v); }
    int32 operator&=(int32 v)   { return _InterlockedAnd(&m_value, v); }
    int32 operator|=(int32 v)   { return _InterlockedOr(&m_value, v); }
    int32 operator++()          { return _InterlockedIncrement(&m_value); }
    int32 operator--()          { return _InterlockedDecrement(&m_value); }
    int32 operator++(int)       { return _InterlockedIncrement(&m_value)-1; }
    int32 operator--(int)       { return _InterlockedDecrement(&m_value)+1; }
    int32 operator=(int32 v)    { swap(v); return v; }
    operator int32() const      { return m_value; }

private:
    volatile LONG m_value;
};

#if defined(ist_env_x64)
class istInterModule atomic_int64
{
public:
    atomic_int64() : m_value(0) {}
    atomic_int64(int64 v) : m_value(v) {}

    int64 swap(int64 v) { return _InterlockedExchange64(&m_value, v); }
    int64 compare_and_swap(int64 v, int64 comp) { return _InterlockedCompareExchange64(&m_value, v, comp); }
    int64 operator+=(int64 v)   { return _InterlockedExchangeAdd64(&m_value, v); }
    int64 operator-=(int64 v)   { return _InterlockedExchangeAdd64(&m_value,-v); }
    int64 operator&=(int64 v)   { return _InterlockedAnd64(&m_value, v); }
    int64 operator|=(int64 v)   { return _InterlockedOr64(&m_value, v); }
    int64 operator++()          { return _InterlockedIncrement64(&m_value); }
    int64 operator--()          { return _InterlockedDecrement64(&m_value); }
    int64 operator++(int)       { return _InterlockedIncrement64(&m_value)-1; }
    int64 operator--(int)       { return _InterlockedDecrement64(&m_value)+1; }
    int64 operator=(int64 v)    { swap(v); return v; }
    operator int64() const      { return m_value; }

private:
    volatile LONGLONG m_value;
};
#endif // ist_env_x64

#if defined(ist_env_x64)
typedef atomic_int64 atomic_ptrint;
#elif defined(ist_env_x86)
typedef atomic_int32 atomic_ptrint;
#endif

#else // ist_env_Windows

class istInterModule atomic_int32
{
public:
    atomic_int32() : m_value(0) {}
    atomic_int32(int32 v) : m_value(v) {}

    int32 swap(int32 v) { return __sync_lock_test_and_set(&m_value, v); }
    int32 compare_and_swap(int32 v, int32 comp) { return __sync_val_compare_and_swap(m_value, comp, v); }
    int32 operator+=(int32 v)   { return __sync_add_and_fetch(&m_value, v); }
    int32 operator-=(int32 v)   { return __sync_sub_and_fetch(&m_value, v); }
    int32 operator&=(int32 v)   { return __sync_and_and_fetch(&m_value, v); }
    int32 operator|=(int32 v)   { return __sync_or_and_fetch(&m_value, v); }
    int32 operator++()          { return __sync_add_and_fetch(&m_value, 1); }
    int32 operator--()          { return __sync_sub_and_fetch(&m_value, 1); }
    int32 operator++(int)       { return __sync_add_and_fetch(&m_value, 1)-1; }
    int32 operator--(int)       { return __sync_sub_and_fetch(&m_value, 1)+1; }
    int32 operator=(int32 v)    { swap(v); return v; }
    operator int32() const      { return m_value; }

private:
    int32 m_value;
};

typedef atomic_int32 atomic_ptr;

#endif // ist_env_Windows

} // namespace ist

#endif // ist_Concurrency_Atomic_h
