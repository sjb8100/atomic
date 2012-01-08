#ifndef __atomic_Game_Character_Enemy__
#define __atomic_Game_Character_Enemy__

#include "Game/Entity.h"
#include "Game/EntityQuery.h"
#include "Attributes.h"
#include "Routine.h"

namespace atomic {


    class IRoutine;

    class Breakable : public IEntity, public Attr_MessageHandler
    {
    typedef IEntity super;
    typedef Attr_MessageHandler mhandler;
    private:
        mat4        m_transform;
        mat4        m_itransform;
        vec4        m_flash_color;
        IRoutine    *m_routine;
        float32     m_health;
        float32     m_delta_damage;
        int         m_past_frame;

    public:
        Breakable()
        : m_transform(), m_routine(NULL), m_health(1.0f), m_delta_damage(0.0f), m_past_frame(0)
        {}

        ~Breakable()
        {
            istSafeDelete(m_routine);
        }

        float32     getHealth() const           { return m_health; }
        const mat4& getTransform() const        { return m_transform; }
        const mat4& getInverseTransform() const { return m_itransform; }
        IRoutine*   getRoutine()                { return m_routine; }
        const vec4& getFlashColor() const       { return m_flash_color; }
        int         getPastFrame() const        { return m_past_frame; }

        void setHealth(float32 v)       { m_health=v; }
        void setTransform(const mat4& v){ m_transform=v; m_itransform=glm::inverse(m_transform); }
        void setRoutine(ROUTINE_CLASSID rcid)
        {
            istSafeDelete(m_routine);
            m_routine = CreateRoutine(rcid);
            if(m_routine) { m_routine->setEntity(this); }
        }

        virtual void update(float32 dt)
        {
            ++m_past_frame;
            updateRoutine(dt);
            updateDamageFlash();
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
                if(m_delta_damage < threthold1) {
                }
                else if(m_delta_damage < threthold2) {
                    float32 d = m_delta_damage - threthold1;
                    m_flash_color = vec4(d/threthold2, d/threthold2, 0.0f, 0.0f) * 0.5f;
                }
                else {
                    float32 d = m_delta_damage - threthold2;
                    m_flash_color = vec4(1.0f, std::max<float32>(1.0f-d, 0.0f), 0.0f, 0.0f) * 0.5f;
                }
            }
            m_delta_damage = 0.0f;
        }


        virtual void asyncupdate(float32 dt)
        {
            asyncupdateRoutine(dt);
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
            atomicDeleteEntity(getHandle());
        }

        virtual bool call(uint32 call_id, const variant &v)
        {
            if(m_routine && m_routine->call(call_id, v)) { }

            switch(call_id) {
            DEFINE_ECALL1(setHealth, float32);
            DEFINE_ECALL1(setRoutine, ROUTINE_CLASSID);
            DEFINE_ECALL1(damage, float32);
            default: return super::call(call_id, v) || mhandler::call(call_id, v);
            }
        }

        virtual bool query(uint32 query_id, variant &v) const
        {
            if(m_routine && m_routine->query(query_id, v)) { return true; }

            switch(query_id) {
                DEFINE_EQUERY(getHealth);
            }
            return false;
        }
    };


} // namespace atomic
#endif // __atomic_Game_Character_Enemy__
