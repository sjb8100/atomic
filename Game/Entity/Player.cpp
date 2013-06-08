﻿#include "stdafx.h"
#include "types.h"
#include "Util.h"
#include "Sound/AtomicSound.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/Renderer.h"
#include "Game/AtomicApplication.h"
#include "Game/AtomicGame.h"
#include "Game/World.h"
#include "Game/SPHManager.h"
#include "Game/Collision.h"
#include "Game/Message.h"
#include "Enemy.h"

namespace atm {

class dpPatch IWeaponry
{
private:
    EntityHandle m_owner;

    istSerializeBlock(
        istSerialize(m_owner)
        )

public:
    IWeaponry() : m_owner(0) {}
    virtual ~IWeaponry() {}
    virtual void update(float32 dt){}
    virtual void asyncupdate(float32 dt) {}
    virtual void draw() {}

    void         setOwner(const IEntity *e) { m_owner = e ? e->getHandle() : 0; }
    IEntity*     getOwner() const { return atmGetEntity(m_owner); }
    EntityHandle getOwnerHandle() const { return m_owner; }
};
typedef IWeaponry IDrive;


class dpPatch Booster : public IDrive
{
typedef IDrive super;
private:
    int32 m_cooldown;

    istSerializeBlock(
        istSerializeBase(super)
        istSerialize(m_cooldown)
        )

public:
    Booster() : m_cooldown(0)
    {
    }

    virtual void asyncupdate(float32 dt)
    {
        m_cooldown = stl::max<int32>(0, m_cooldown-1);

        IEntity *owner = getOwner();
        if(owner) {
            vec4 move = vec4(atmGetIngameInputs().getMove()*0.01f, 0.0f, 0.0f);
            vec4 pos, vel;
            atmQuery(owner, getPosition, pos);
            atmQuery(owner, getVelocity, vel);
            pos += move;
            pos += vel;
            vel *= 0.96f;
            pos.z = 0.0f;
            if(m_cooldown==0 && atmGetIngameInputs().isButtonTriggered(0)) {
                vel += move * 2.0f;
                m_cooldown = 10;
            }
            atmCall(owner, setPosition, pos);
            atmCall(owner, setVelocity, vel);
        }
    }

    virtual void draw() {}
};
atmExportClass(atm::Booster);


class dpPatch Blinker : public IDrive
{
typedef IDrive super;
private:
    enum {
        St_Neutral,
        St_Targetting,
    };

    int32 m_state;
    vec4 m_blink_pos;

    istSerializeBlock(
        istSerializeBase(super)
        istSerialize(m_state)
        istSerialize(m_blink_pos)
        )
public:
    Blinker() : m_state(St_Neutral), m_blink_pos()
    {
    }

    virtual void asyncupdate(float32 dt)
    {
        IEntity *owner = getOwner();
        if(owner) {
            vec4 move = vec4(atmGetIngameInputs().getMove()*0.01f, 0.0f, 0.0f);
            vec4 pos, vel;
            atmQuery(owner, getPosition, pos);
            atmQuery(owner, getVelocity, vel);

            switch(m_state) {
            case St_Neutral:
                {
                    pos += move;
                    if(atmGetIngameInputs().isButtonTriggered(0)) {
                        m_blink_pos = pos;
                        m_state = St_Targetting;
                    }
                }
                break;
            case St_Targetting:
                {
                    m_blink_pos += move*4.0f;
                    if(!atmGetIngameInputs().isButtonPressed(0)) {
                        pos = m_blink_pos;
                        m_state = St_Neutral;
                    }
                }
                break;
            }
            pos += vel;
            vel *= 0.96f;
            pos.z = 0.0f;
            atmCall(owner, setPosition, pos);
            atmCall(owner, setVelocity, vel);
        }
    }
};
atmExportClass(atm::Blinker);


class dpPatch Penetrator : public IDrive
{
typedef IDrive super;
private:
    istSerializeBlock(
        istSerializeBase(super)
        )
public:
};
atmExportClass(atm::Penetrator);


class dpPatch TimeWarp : public IDrive
{
typedef IDrive super;
private:
    istSerializeBlock(
        istSerializeBase(super)
        )
public:
};
atmExportClass(atm::TimeWarp);



class dpPatch BarrierGenerator : public IWeaponry
{
typedef IWeaponry super;
private:
    EntityHandle m_barrier;

    istSerializeBlock(
        istSerializeBase(super)
        istSerialize(m_barrier)
        )
public:
    BarrierGenerator() : m_barrier(0)
    {
    }

    virtual void update(float32 dt)
    {
        vec4 center_force;
        IEntity *barrier = atmGetEntity(m_barrier);
        IEntity *owner = getOwner();
        if(!barrier) {
            IEntity *e = atmCreateEntity(Barrier);
            m_barrier = e->getHandle();
            atmCall(e, setOwner, getOwnerHandle());
            atmQuery(owner, getPosition, center_force);
        }
        else {
            if(barrier && owner) {
                if(!atmGetIngameInputs().isButtonPressed(1)) {
                    vec4 barrier_pos;
                    atmQuery(owner, getPosition, barrier_pos);
                    atmCall(barrier, setPosition, barrier_pos);
                }
            }
            atmQuery(barrier, getPosition, center_force);
        }

        {
            psym::PointForce force;
            force.x = center_force.x;
            force.y = center_force.y;
            force.z = center_force.z;
            force.strength = 2.4f;
            atmGetSPHManager()->addForce(force);
        }
    }

    virtual void asyncupdate(float32 dt)
    {
        IEntity *owner = getOwner();
        IEntity *barrier = atmGetEntity(m_barrier);
        if(owner && barrier) {
        }
    }

    virtual void draw() {}
};
atmExportClass(atm::BarrierGenerator);


class dpPatch GravityMineLauncher : public IWeaponry
{
typedef IWeaponry super;
private:
    istSerializeBlock(
        istSerializeBase(super)
        )
public:
};
atmExportClass(atm::GravityMineLauncher);


class dpPatch Catapult : public IWeaponry
{
typedef IWeaponry super;
private:
    istSerializeBlock(
        istSerializeBase(super)
        )
public:
};
atmExportClass(atm::Catapult);


class dpPatch Barrier
    : public Breakable
    , public TAttr_TransformMatrix<Attr_Translate>
    , public Attr_Collision
{
typedef Barrier                                 this_t;
typedef Breakable                               super;
typedef TAttr_TransformMatrix<Attr_Translate>   transform;
typedef Attr_Collision                          collision;
private:
    EntityHandle    m_owner;
    float32         m_life;
    Attr_Collision  m_collision;

    istSerializeBlock(
        istSerializeBase(super)
        istSerializeBase(transform)
        istSerializeBase(collision)
        istSerialize(m_owner)
        istSerialize(m_life)
        istSerialize(m_collision)
        )

public:
    atmECallBlock(
        atmECallSuper(super)
        atmECallSuper(transform)
        atmMethodBlock(
        atmECall(setOwner)
        atmECall(getOwner)
        )
    )

public:
    Barrier() : m_owner(0), m_life(100.0f)
    {
    }

    void            setOwner(EntityHandle v)    { m_owner=v; }
    EntityHandle    getOwner() const            { return m_owner; }

    virtual void initialize()
    {
        initializeCollision(getHandle());
        setCollisionShape(CS_Sphere);
        setCollisionFlags(CF_SPH_Sender);
        getCollisionSphere().pos_r.w = 0.125f*3.0f;
    }

    virtual void finalize()
    {
    }

    virtual void update(float32 dt)
    {
    }

    virtual void asyncupdate(float32 dt)
    {
        super::update(dt);

        transform::updateTransformMatrix();
        updateCollision(transform::getTransform());
    }

    virtual void draw()
    {
    }
};
atmImplementEntity(Barrier);
atmExportClass(atm::Barrier);


class dpPatch Player
    : public Breakable
    , public TAttr_TransformMatrix< TAttr_RotateSpeed<Attr_DoubleAxisRotation> >
    , public Attr_Collision
{
typedef Player      this_t;
typedef Breakable   super;
typedef TAttr_TransformMatrix< TAttr_RotateSpeed<Attr_DoubleAxisRotation> > transform;
typedef Attr_Collision collision;
private:
    static const PSET_RID pset_id = PSET_SPHERE_SMALL;

    vec4 m_vel;
    IDrive      *m_drive;
    IWeaponry   *m_weapon;

    vec4 m_lightpos[1];
    vec4 m_lightvel[1];

    istSerializeBlock(
        istSerializeBase(super)
        istSerializeBase(transform)
        istSerializeBase(collision)
        istSerialize(m_vel)
        istSerialize(m_drive)
        istSerialize(m_weapon)
        istSerialize(m_lightpos)
        istSerialize(m_lightvel)
        )

public:
    enum {
        Drive_Booster,
        Drive_Blinker,
        Drive_Penetrator,
        Drive_TimeWarp,
    };
    enum {
        Weponry_Barrier,
        Weponry_GravityMineLauncher,
        Weponry_Catapult,
    };

    atmECallBlock(
        atmECallSuper(super)
        atmECallSuper(transform)
        atmECallSuper(collision)
        atmMethodBlock(
        atmECall(getVelocity)
        atmECall(setVelocity)
        atmECall(setDrive)
        atmECall(setWeapon)
        )
    )

public:
    Player()
        : m_vel()
        , m_drive(nullptr), m_weapon(nullptr)
    {
        wdmScope(
            wdmString path = wdmFormat("Player/0x%p", this);
            super::addDebugNodes(path);
            transform::addDebugNodes(path);
            wdmAddNode(path+"/setDrive()", &Player::setDrive, this);
            wdmAddNode(path+"/setWeapon()", &Player::setWeapon, this);
        )
    }

    ~Player()
    {
        istSafeDelete(m_drive);
        istSafeDelete(m_weapon);
        wdmEraseNode(wdmFormat("Player/0x%p", this));
    }

    const vec4& getVelocity() const { return m_vel; }
    void setVelocity(const vec4 &v) { m_vel=v; }

    void setDrive(int32 id) // id: Drive_Booster, etc
    {
        IDrive *old_drive = m_drive;
        switch(id) {
        case Drive_Booster:
            m_drive = istNew(Booster)();
            break;
        case Drive_Blinker:
            m_drive = istNew(Blinker)();
            break;
        default:
            istAssert(false && "unknown drive type");
            return;
        }
        m_drive->setOwner(this);
        istSafeDelete(old_drive);
    }

    void setWeapon(int32 id) // id: Weponry_Barrier, etc
    {
        IWeaponry *old_weapon = m_weapon;
        switch(id) {
        case Weponry_Barrier:
            m_weapon = istNew(BarrierGenerator)();
            break;
        case Weponry_GravityMineLauncher:
            m_weapon = istNew(GravityMineLauncher)();
            break;
        default:
            istAssert(false && "unknown weapon type");
            return;
        }
        m_weapon->setOwner(this);
        istSafeDelete(old_weapon);
    }

    virtual void initialize() override
    {
        super::initialize();

        setDrive(Drive_Booster);
        setWeapon(Weponry_Barrier);

        initializeCollision(getHandle());
        setCollisionShape(CS_Sphere);
        getCollisionSphere().pos_r.w = 0.125f*0.5f;

        setLife(500.0f);
        setAxis1(GenRandomUnitVector3());
        setAxis2(GenRandomUnitVector3());
        setRotateSpeed1(1.4f);
        setRotateSpeed2(1.4f);

        for(uint32 i=0; i<_countof(m_lightpos); ++i) {
            m_lightpos[i] = GenRandomVector3() * 1.0f;
            m_lightpos[i].z = std::abs(m_lightpos[i].z);
        }
    }

    virtual void update(float32 dt) override
    {
        super::update(dt);
        if(m_drive)  {m_drive->update(dt); }
        if(m_weapon) {m_weapon->update(dt); }

        // 流体パーティクルが 10000 以下なら追加
        if(atmGetSPHManager()->getNumParticles()<10000) {
            psym::Particle particles[16];
            for(size_t i=0; i<_countof(particles); ++i) {
                vec4 rd = glm::normalize(vec4(atmGenRandFloat()-0.5f, atmGenRandFloat()-0.5f, 0.0f, 0.0f));
                istAlign(16) vec4 pos = getPosition() + (rd * (atmGenRandFloat()*0.2f+0.4f));
                psym::simdvec4 poss = (psym::simdvec4&)pos;
                particles[i].position = poss;
                particles[i].velocity = _mm_set1_ps(0.0f);
            }
            atmGetSPHManager()->addFluid(&particles[0], _countof(particles));
        }
    }

    void asyncupdate(float32 dt) override
    {
        super::asyncupdate(dt);
        if(m_drive)  {m_drive->asyncupdate(dt); }
        if(m_weapon) {m_weapon->asyncupdate(dt); }

        updateLights();

        transform::updateRotate(dt);
        transform::updateTransformMatrix();
        collision::updateCollisionByParticleSet(pset_id, getTransform(), 0.5f);
    }

    void updateLights()
    {
        vec4 diff[4] = {
            vec4( 0.0f, 0.0f, 0.0f, 0.0f),
            vec4(-0.4f, 0.4f, 0.0f, 0.0f),
            vec4(-0.4f,-0.4f, 0.0f, 0.0f),
            vec4( 0.4f,-0.4f, 0.0f, 0.0f),
        };
        for(uint32 i=0; i<_countof(m_lightpos); ++i) {
            vec4 &pos = m_lightpos[i];
            vec4 &vel = m_lightvel[i];
            vel *= 0.985f;
            vel += glm::normalize(getPosition()+diff[i]-pos) * 0.005f;
            pos += vel;
            pos.z = 0.5f;
        }
    }

    virtual void draw() override
    {
        {
            PointLight l;
            l.setPosition(getPosition()+vec4(0.0f, 0.0f, 0.3f, 0.0f));
            l.setColor(vec4(0.3f, 0.2f, 1.0f, 1.0f));
            l.setRadius(1.0f);
            atmGetLights()->addLight(l);
        }
        for(uint32 i=0; i<_countof(m_lightpos); ++i) {
            vec4 &pos = m_lightpos[i];
            PointLight l;
            l.setPosition(pos);
            l.setColor(vec4(0.45f, 0.45f, 0.6f, 1.0f) + vec4(sinf(pos.x), sinf(pos.y), cosf(pos.x+pos.y), 0.0f)*0.1f);
            l.setRadius(1.2f);
            atmGetLights()->addLight(l);
        }
        {
            PSetInstance inst;
            inst.diffuse = vec4(0.6f, 0.6f, 0.6f, 50.0f);
            inst.glow = vec4(0.2f, 0.0f, 1.0f, 0.0f);
            inst.flash = vec4();
            inst.elapsed = (float32)getPastFrame();
            inst.appear_radius = 1000.0f;
            inst.translate = getTransform();
            atmGetSPHRenderer()->addPSetInstance(pset_id, inst);
        }
        //{
        //    IndivisualParticle particles;
        //    particles.position = getPosition()+vec4(0.3f, 0.3f, 0.05f, 0.0f);
        //    particles.color = vec4(0.6f, 0.6f, 0.6f, 50.0f);
        //    particles.glow = vec4(0.15f, 0.15f, 0.3f, 1.0f);
        //    particles.scale = 3.0f;
        //    atmGetParticleRenderer()->addParticle(&particles, 1);
        //}
    }

    virtual void destroy() override
    {
        atmGetSPHManager()->addFluid(pset_id, getTransform());
        atmPlaySE(SE_CHANNEL5, SE_EXPLOSION5, getPosition(), true);
        super::destroy();
    }

    virtual void eventCollide(const CollideMessage *m) override
    {
        // 押し返し
        vec4 v = m->direction * (m->direction.w * 0.2f);
        m_vel += v;
        m_vel.z = 0.0f;
        m_vel.w = 0.0f;

        damage(m->direction.w * 100.0f);
    }
};
atmImplementEntity(Player);
atmExportClass(atm::Player);

} // namespace atm
