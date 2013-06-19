﻿#ifndef atm_Game_Entity_Enemy_h
#define atm_Game_Entity_Enemy_h

#include "Game/Entity.h"
#include "Game/EntityQuery.h"
#include "Attributes.h"
#include "Routine.h"

namespace atm {


class IRoutine;

class Breakable
    : public IEntity
    , public Attr_MessageHandler
{
typedef Breakable this_t;
typedef IEntity super;
typedef Attr_MessageHandler mhandler;
private:
    vec4        m_flash_color;
    IRoutine    *m_routine;
    float32     m_health;
    float32     m_delta_damage;
    int32       m_past_frame;

    istSerializeBlock(
        istSerializeBase(super)
        istSerializeBase(mhandler)
        istSerialize(m_flash_color)
        istSerialize(m_routine)
        istSerialize(m_health)
        istSerialize(m_delta_damage)
        istSerialize(m_past_frame)
        )

public:
    atmECallBlock(
        atmECallDelegate(m_routine)
        atmMethodBlock(
        atmECall(getLife)
        atmECall(setLife)
        atmECall(setRoutine)
        atmECall(damage)
        )
        atmECallSuper(super)
        atmECallSuper(mhandler)
    )

    wdmScope(
    void addDebugNodes(const wdmString &path)
    {
        wdmAddNode(path+"/m_health", &m_health);
        wdmAddNode(path+"/damage()", &Breakable::damage, this);
        wdmAddNode(path+"/destroy()", &Breakable::destroy, this);
    }
    )

public:
    Breakable()
    : m_routine(NULL), m_health(1.0f), m_delta_damage(0.0f), m_past_frame(0)
    {}

    ~Breakable()
    {
        istSafeDelete(m_routine);
    }

    float32     getLife() const             { return m_health; }
    IRoutine*   getRoutine()                { return m_routine; }
    const vec4& getFlashColor() const       { return m_flash_color; }
    int32       getPastFrame() const        { return m_past_frame; }

    void setLife(float32 v)       { m_health=v; }
    void setRoutine(RoutineClassID rcid)
    {
        istSafeDelete(m_routine);
        m_routine = CreateRoutine(rcid);
        if(m_routine) { m_routine->setEntity(this); }
    }

    virtual void update(float32 dt)
    {
        ++m_past_frame;
        updateRoutine(dt);
    }

    virtual void updateRoutine(float32 dt)
    {
        if(m_routine) { m_routine->update(dt); }
    }

    virtual void updateDamageFlash()
    {
        m_flash_color = vec4();
        if(m_past_frame % 4 < 2) {
            const float32 threthold1 = 0.05f;
            const float32 threthold2 = 1.0f;
            const float32 threthold3 = 10.0f;
            if(m_delta_damage < threthold1) {
            }
            else if(m_delta_damage < threthold2) {
                float32 d = m_delta_damage - threthold1;
                float32 r = threthold2 - threthold1;
                m_flash_color = vec4(d/r, d/r, 0.0f, 0.0f);
            }
            else if(m_delta_damage) {
                float32 d = m_delta_damage - threthold2;
                float32 r = threthold3 - threthold2;
                m_flash_color = vec4(1.0f, stl::max<float32>(1.0f-d/r, 0.0f), 0.0f, 0.0f);
            }
            m_flash_color *= 0.25f;
        }
        m_delta_damage = 0.0f;
    }


    virtual void asyncupdate(float32 dt)
    {
        asyncupdateRoutine(dt);
        updateDamageFlash();
    }

    virtual void asyncupdateRoutine(float32 dt)
    {
        if(m_routine) { m_routine->asyncupdate(dt); }
    }

    virtual void damage(float32 d)
    {
        if(m_health > 0.0f) {
            m_health -= d;
            m_delta_damage += d;
            if(m_health <= 0.0f) {
                destroy();
            }
        }
    }

    virtual void destroy()
    {
        atmDeleteEntity(getHandle());
    }
};


inline size_t SweepDeadEntities(stl::vector<EntityHandle> &cont)
{
    size_t ret = 0;
    for(size_t i=0; i<cont.size(); ++i) {
        EntityHandle v = cont[i];
        if(v) {
            if(atmGetEntity(v)==nullptr) {
                cont[i] = 0;
            }
            else {
                ++ret;
            }
        }
    }
    return ret;
}

template<size_t L>
inline size_t SweepDeadEntities(EntityHandle (&cont)[L])
{
    size_t ret = 0;
    for(size_t i=0; i<L; ++i) {
        EntityHandle v = cont[i];
        if(v) {
            if(atmGetEntity(v)==nullptr) {
                cont[i] = 0;
            }
            else {
                ++ret;
            }
        }
    }
    return ret;
}

inline size_t SweepDeadEntities(EntityHandle &v)
{
    if(v) {
        if(atmGetEntity(v)==nullptr) {
            v = 0;
        }
        else {
            return 1;
        }
    }
    return 0;
}

template<class F>
inline void EachEntities(stl::vector<EntityHandle> &cont, const F &f)
{
    for(size_t i=0; i<cont.size(); ++i) {
        if(cont[i]) { f(cont[i]); }
    }
}

} // namespace atm
#endif // atm_Game_Entity_Enemy_h
