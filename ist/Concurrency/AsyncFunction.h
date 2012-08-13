﻿#ifndef __ist_Concurrency_AsyncFunction_h__
#define __ist_Concurrency_AsyncFunction_h__

#include "ist/Base/Generics.h"
#include "ist/Concurrency/TaskScheduler.h"

namespace ist {

class AsyncFunctionBase : public Task
{
public:
    ~AsyncFunctionBase() { wait(); }
    void start() { TaskScheduler::getInstance()->enqueue(this); }
};

template<class Ret=void, class Arg1=void, class Arg2=void, class Arg3=void, class Arg4=void>
class AsyncFunction;

template<class Class, class Ret=void, class Arg1=void, class Arg2=void, class Arg3=void, class Arg4=void>
class AsyncMethod;

template<class Class, class Ret=void, class Arg1=void, class Arg2=void, class Arg3=void, class Arg4=void>
class AsyncConstMethod;


template<class Arg>
struct ArgHolder
{
    ArgHolder() {}
    ArgHolder(Arg v) : m_value(v) {}
    operator Arg() const { return m_value; }
    Arg m_value;
};
template<class Arg>
struct ArgHolder<const Arg>
{
    ArgHolder() {}
    ArgHolder(const Arg v) : m_value(v) {}
    operator Arg() const { return m_value; }
    Arg m_value;
};
template<class Arg>
struct ArgHolder<Arg&>
{
    ArgHolder() {}
    ArgHolder(Arg &v) : m_value(&v) {}
    operator Arg() const { return *m_value; }
    Arg *m_value;
};
template<class Arg>
struct ArgHolder<const Arg&>
{
    ArgHolder() {}
    ArgHolder(const Arg &v) : m_value(&v) {}
    operator Arg() const { return *m_value; }
    const Arg *m_value;
};



// async function: arg 0
template<class Ret>
class AsyncFunction<Ret> : public AsyncFunctionBase
{
public:
    typedef Ret (*Func)();
    typedef ArgHolder<Ret> RetH;

    AsyncFunction() {}
    AsyncFunction(Func f) { start(f); }
    void start(Func f) { m_func=f; AsyncFunctionBase::start(); }
    void exec() { m_ret=m_func(); }
    Ret getValue() { wait(); return m_ret; }
private:
    Func m_func;
    RetH m_ret;
};

template<>
class AsyncFunction<> : public AsyncFunctionBase
{
public:
    typedef void (*Func)();

    AsyncFunction() {}
    AsyncFunction(Func f) { start(f); }
    void start(Func f) { m_func=f; AsyncFunctionBase::start(); }
    void exec() { m_func(); }
    void getValue() { wait(); }
private:
    Func m_func;
};

// async function: arg 1
template<class Ret, class Arg1>
class AsyncFunction<Ret, Arg1> : public AsyncFunctionBase
{
public:
    typedef Ret (*Func)(Arg1);
    typedef ArgHolder<Ret> RetH;
    typedef ArgHolder<Arg1> Arg1H;

    AsyncFunction() {}
    AsyncFunction(Func f, Arg1 a1) { start(f, a1); }
    void start(Func f, Arg1 a1) { m_func=f; m_arg=a1; AsyncFunctionBase::>start(); }
    void exec() { m_ret=m_func(m_arg1); }
    Ret getValue() { wait(); return m_ret; }
private:
    Func m_func;
    RetH m_ret; Arg1H m_arg1;
};

template<class Arg1>
class AsyncFunction<void, Arg1> : public AsyncFunctionBase
{
public:
    typedef void (*Func)(Arg1);
    typedef ArgHolder<Arg1> Arg1H;

    AsyncFunction() {}
    AsyncFunction(Func f, Arg1 a1) { start(f, a1); }
    void start(Func f, Arg1 a1) { m_func=f; m_arg=a1; AsyncFunctionBase::start(); }
    void exec() { m_func(m_arg1); }
    void getValue() { wait(); }
private:
    Func m_func;
    Arg1H m_arg1;
};


// async method: arg 0
template<class Class, class Ret>
class AsyncMethod<Class, Ret> : public AsyncFunctionBase
{
public:
    typedef Ret (Class::*Func)();
    typedef ArgHolder<Ret> RetH;

    AsyncMethod() {}
    AsyncMethod(Func f, Class &o) { start(f, o); }
    void start(Func f, Class &o) { m_func=f; m_obj=&o; AsyncFunctionBase::start(); }
    void exec() { m_ret=(m_obj->*m_func)(); }
    Ret getValue() { wait(); return m_ret; }
private:
    Func m_func;
    Class *m_obj;
    RetH m_ret;
};

template<class Class>
class AsyncMethod<Class, void> : public AsyncFunctionBase
{
public:
    typedef void (Class::*Func)();

    AsyncMethod() {}
    AsyncMethod(Func f, Class &o) { start(f, o); }
    void start(Func f, Class &o) { m_func=f; m_obj=&o; AsyncFunctionBase::start(); }
    void exec() { (m_obj->*m_func)(); }
    void getValue() { wait(); }
private:
    Func m_func;
    Class *m_obj;
};


// async method: arg 1
template<class Class, class Ret, class Arg1>
class AsyncMethod<Class, Ret, Arg1> : public AsyncFunctionBase
{
public:
    typedef Ret (Class::*Func)(Arg1);
    typedef ArgHolder<Ret> RetH;
    typedef ArgHolder<Arg1> Arg1H;

    AsyncMethod() {}
    AsyncMethod(Func f, Class &o, Arg1 a1) { start(f, o, a1); }
    void start(Func f, Class &o, Arg1 a1) { m_func=f; m_obj=&o; m_arg1=a1; AsyncFunctionBase::start(); }
    void exec() { m_ret=(m_obj->*m_func)(m_arg1); }
    Ret getValue() { wait(); return m_ret; }
private:
    Func m_func;
    Class *m_obj;
    RetH m_ret; Arg1H m_arg1;
};

template<class Class, class Arg1>
class AsyncMethod<Class, void, Arg1> : public AsyncFunctionBase
{
public:
    typedef void (Class::*Func)(Arg1);
    typedef ArgHolder<Arg1> Arg1H;

    AsyncMethod() {}
    AsyncMethod(Func f, Class &o, Arg1 a1) { start(f, o, a1); }
    void start(Func f, Class &o, Arg1 a1) { m_func=f; m_obj=&o; m_arg1=a1; AsyncFunctionBase::start(); }
    void exec() { (m_obj->*m_func)(m_arg1); }
    void getValue() { wait(); }
private:
    Func m_func;
    Class *m_obj;
    Arg1H m_arg1;
};


// async const method: arg 0
template<class Class, class Ret>
class AsyncConstMethod<Class, Ret> : public AsyncFunctionBase
{
public:
    typedef Ret (Class::*Func)() const;
    typedef ArgHolder<Ret> RetH;

    AsyncConstMethod() {}
    AsyncConstMethod(Func f, const Class &o) { start(f, o); }
    void start(Func f, const Class &o) { m_func=f; m_obj=&o; AsyncFunctionBase::start(); }
    void exec() { m_ret=(m_obj->*m_func)(); }
    Ret getValue() { wait(); return m_ret; }
private:
    Func m_func;
    const Class *m_obj;
    RetH m_ret;
};

template<class Class>
class AsyncConstMethod<Class, void> : public AsyncFunctionBase
{
public:
    typedef void (Class::*Func)() const;

    AsyncConstMethod() {}
    AsyncConstMethod(Func f, const Class &o) { start(f, o); }
    void start(Func f, const Class &o) { m_func=f; m_obj=&o; AsyncFunctionBase::start(); }
    void exec() { (m_obj->*m_func)(); }
    void getValue() { wait(); }
private:
    Func m_func;
    const Class *m_obj;
};


// async const method: arg 1
template<class Class, class Ret, class Arg1>
class AsyncConstMethod<Class, Ret, Arg1> : public AsyncFunctionBase
{
public:
    typedef Ret (Class::*Func)(Arg1) const;
    typedef ArgHolder<Ret> RetH;
    typedef ArgHolder<Arg1> Arg1H;

    AsyncConstMethod() {}
    AsyncConstMethod(Func f, const Class &o, Arg1 a1) { start(f, o, a1); }
    void start(Func f, const Class &o, Arg1 a1) { m_func=f; m_obj=&o; m_arg1=a1; AsyncFunctionBase::start(); }
    void exec() { m_ret=(m_obj->*m_func)(m_arg1); }
    Ret getValue() { wait(); return m_ret; }
private:
    Func m_func;
    const Class *m_obj;
    RetH m_ret; Arg1H m_arg1;
};

template<class Class, class Arg1>
class AsyncConstMethod<Class, void, Arg1> : public AsyncFunctionBase
{
public:
    typedef void (Class::*Func)(Arg1) const;
    typedef ArgHolder<Arg1> Arg1H;

    AsyncConstMethod() {}
    AsyncConstMethod(Func f, const Class &o, Arg1 a1) { start(f, o, a1); }
    void start(Func f, const Class &o, Arg1 a1) { m_func=f; m_obj=&o; m_arg1=a1; AsyncFunctionBase::start(); }
    void exec() { (m_obj->*m_func)(m_arg1); }
    void getValue() { wait(); }
private:
    Func m_func;
    const Class *m_obj;
    Arg1H m_arg1;
};



} // namespace ist

#endif // __ist_Concurrency_AsyncFunction_h__
