﻿#ifndef atm_Game_Entity_Attributes_h
#define atm_Game_Entity_Attributes_h

#include "Util.h"
#include "Game/Collision.h"
#include "Game/SPHManager.h"
#include "Graphics/Renderer.h"
#include "psym/psym.h"

struct sphFluidMessage;
typedef psym::Particle FluidMessage;


namespace atm {

class Attr_Null
{
    istSerializeBlock()
public:
    atmECallBlock()
    wdmScope( void addDebugNodes(const wdmString &path) {} )
};

class Attr_RefCount
{
private:
    uint32 m_refcount;

    istSerializeBlock(
        istSerialize(m_refcount)
    )
protected:
    void setRefCount(uint32 v) { m_refcount=v; }

public:
    atmECallBlock(
        atmMethodBlock(
        atmECall(setRefCount)
        atmECall(incRefCount)
        atmECall(decRefCount)
        atmECall(getRefCount)
        )
    )

    wdmScope(
    void addDebugNodes(const wdmString &path)
    {
        wdmAddNode(path+"/m_refcount", (const uint32*)&m_refcount);
    }
    )

    Attr_RefCount() : m_refcount(0) {}
    uint32 getRefCount() const  { return m_refcount; }
    uint32 incRefCount()        { return ++m_refcount; }
    uint32 decRefCount()        { return --m_refcount; }
};


class Attr_Translate
{
protected:
    vec3 m_pos;

    istSerializeBlock(
        istSerialize(m_pos)
    )
public:
    atmECallBlock(
        atmMethodBlock(
        atmECall(getPosition)
        atmECall(setPosition)
        )
    )

    wdmScope(
    void addDebugNodes(const wdmString &path)
    {
        wdmAddNode(path+"/m_pos",   &m_pos,   -3.0f, 3.0f);
    }
    )

public:
    Attr_Translate() {}
    const vec3& getPosition() const { return m_pos; }
    void setPosition(const vec3& v) { m_pos=v; }

    mat4 computeMatrix() const
    {
        mat4 mat;
        mat = glm::translate(mat, m_pos);
        return mat;
    }
};

class Attr_Transform
{
private:
    vec3 m_pivot;
    vec3 m_pos;
    vec3 m_scale;
    vec3 m_axis;
    float32 m_rot;

    istSerializeBlock(
        istSerialize(m_pivot)
        istSerialize(m_scale)
        istSerialize(m_axis)
        istSerialize(m_rot)
    )
public:
    atmECallBlock(
        atmMethodBlock(
        atmECall(getPivot)
        atmECall(setPivot)
        atmECall(getPosition)
        atmECall(setPosition)
        atmECall(getScale)
        atmECall(setScale)
        atmECall(getAxis)
        atmECall(setAxis)
        atmECall(getRotate)
        atmECall(setRotate)
        )
    )

    wdmScope(
    void addDebugNodes(const wdmString &path)
    {
        wdmAddNode(path+"/m_pivot", &m_pivot, -3.0f, 3.0f);
        wdmAddNode(path+"/m_pos",   &m_pos,   -3.0f, 3.0f);
        wdmAddNode(path+"/m_scale", &m_scale,  0.001f, 4.0f);
        wdmAddNode(path+"/m_rot",   &m_rot,    0.0f, 360.0f);
    }
    )

public:
    Attr_Transform()
        : m_scale(1.0f, 1.0f, 1.0f)
        , m_axis(0.0f, 0.0f, 1.0f)
        , m_rot(0.0f)
    {}

    const vec3& getPivot() const    { return m_pivot; }
    const vec3& getPosition() const { return m_pos; }
    const vec3& getScale() const    { return m_scale; }
    const vec3& getAxis() const     { return m_axis; }
    float32 getRotate() const       { return m_rot; }
    void setPivot(const vec3& v)    { m_pivot=v; }
    void setPosition(const vec3& v) { m_pos=v; }
    void setScale(const vec3& v)    { m_scale=v; }
    void setAxis(const vec3& v)     { m_axis=v; }
    void setRotate(float32 v)       { m_rot=v; }

    mat4 computeMatrix() const
    {
        mat4 mat;
        mat = glm::translate(mat, m_pos);
        mat = glm::rotate(mat, m_rot, m_axis);
        mat = glm::scale(mat, m_scale);
        mat = glm::translate(mat, -m_pivot);
        return mat;
    }

    void update(float32 dt) {}
    void asyncupdate(float32 dt) {}
};

class Attr_Orientation
{
private:
    vec3 m_pivot;
    vec3 m_pos;
    vec3 m_scale;
    vec3 m_oriantation;
    vec3 m_up;

    istSerializeBlock(
        istSerialize(m_pivot)
        istSerialize(m_pos)
        istSerialize(m_scale)
        istSerialize(m_oriantation)
        istSerialize(m_up)
    )
public:
    atmECallBlock(
        atmMethodBlock(
        atmECall(getPivot)
        atmECall(setPivot)
        atmECall(getPosition)
        atmECall(setPosition)
        atmECall(getScale)
        atmECall(setScale)
        atmECall(getOrientation)
        atmECall(setOrientation)
        atmECall(getUpVector)
        atmECall(setUpVector)
        )
    )

    wdmScope(
    void addDebugNodes(const wdmString &path)
    {
        wdmAddNode(path+"/m_pivot", &m_pivot, -3.0f, 3.0f);
        wdmAddNode(path+"/m_pos",   &m_pos, -3.0f, 3.0f);
        wdmAddNode(path+"/m_scale", &m_scale, 0.001f, 4.0f);
        wdmAddNode(path+"/m_oriantation", this, &Attr_Orientation::getOrientation, &Attr_Orientation::setOrientation);
        wdmAddNode(path+"/m_up", &m_up, 0.0f, 360.0f);
    }
    )

public:
    Attr_Orientation()
        : m_scale(1.0f, 1.0f, 1.0f)
        , m_oriantation(1.0f, 0.0f, 0.0f)
        , m_up(1.0f, 0.0f, 0.0f)
    {}

    const vec3& getPivot() const        { return m_pivot; }
    const vec3& getPosition() const     { return m_pos; }
    const vec3& getScale() const        { return m_scale; }
    const vec3& getOrientation() const  { return m_oriantation; }
    const vec3& getUpVector() const     { return m_up; }
    void setPivot(const vec3& v)        { m_pivot=v; }
    void setPosition(const vec3& v)     { m_pos=v; }
    void setScale(const vec3& v)        { m_scale=v; }
    void setOrientation(const vec3& v)  { m_oriantation=glm::normalize(v); }
    void setUpVector(const vec3& v)     { m_up=v; }

    mat4 computeMatrix() const
    {
        mat4 mat;
        mat = glm::translate(mat, m_pos);
        mat *= glm::orientation(m_oriantation, m_up);
        mat = glm::scale(mat, m_scale);
        mat = glm::translate(mat, -m_pivot);
        return mat;
    }

    void update(float32 dt) {}
    void asyncupdate(float32 dt) {}
};

class Attr_DoubleAxisRotation
{
private:
    vec3 m_pivot;
    vec3 m_pos;
    vec3 m_scale;
    vec3 m_axis1;
    vec3 m_axis2;
    float32 m_rot1;
    float32 m_rot2;

    istSerializeBlock(
        istSerialize(m_pivot)
        istSerialize(m_pos)
        istSerialize(m_scale)
        istSerialize(m_axis1)
        istSerialize(m_axis2)
        istSerialize(m_rot1)
        istSerialize(m_rot2)
        )

public:
    atmECallBlock(
        atmMethodBlock(
        atmECall(getPivot)
        atmECall(setPivot)
        atmECall(getPosition)
        atmECall(setPosition)
        atmECall(getScale)
        atmECall(setScale)
        atmECall(getAxis1)
        atmECall(setAxis1)
        atmECall(getAxis2)
        atmECall(setAxis2)
        atmECall(getRotate1)
        atmECall(setRotate1)
        atmECall(getRotate2)
        atmECall(setRotate2)
        )
    )

    wdmScope(
    void addDebugNodes(const wdmString &path)
    {
        wdmAddNode(path+"/m_pivot", &m_pivot, -3.0f, 3.0f);
        wdmAddNode(path+"/m_pos", &m_pos, -3.0f, 3.0f);
        wdmAddNode(path+"/m_scale", &m_scale, 0.001f, 4.0f);
        wdmAddNode(path+"/m_rot1", &m_rot1, 0.0f, 360.0f);
        wdmAddNode(path+"/m_rot2", &m_rot2, 0.0f, 360.0f);
    }
    )

public:
    Attr_DoubleAxisRotation()
        : m_scale(1.0f, 1.0f, 1.0f)
        , m_axis1(0.0f, 1.0f, 0.0f)
        , m_axis2(0.0f, 0.0f, 1.0f)
        , m_rot1(0.0f), m_rot2(0.0f)
    {
    }

    const vec3& getPivot() const    { return m_pivot; }
    const vec3& getPosition() const { return m_pos; }
    const vec3& getScale() const    { return m_scale; }
    const vec3& getAxis1() const    { return m_axis1; }
    const vec3& getAxis2() const    { return m_axis2; }
    float32 getRotate1() const      { return m_rot1; }
    float32 getRotate2() const      { return m_rot2; }

    void setPivot(const vec3& v)    { m_pivot=v; }
    void setPosition(const vec3& v) { m_pos=v; }
    void setScale(const vec3& v)    { m_scale=v; }
    void setAxis1(const vec3& v)    { m_axis1=v; }
    void setAxis2(const vec3& v)    { m_axis2=v; }
    void setRotate1(float32 v)      { m_rot1=v; }
    void setRotate2(float32 v)      { m_rot2=v; }

    mat4 computeMatrix() const
    {
        mat4 mat;
        mat = glm::translate(mat, m_pos);
        mat = glm::rotate(mat, m_rot2, m_axis2);
        mat = glm::rotate(mat, m_rot1, m_axis1);
        mat = glm::scale(mat, m_scale);
        mat = glm::translate(mat, -m_pivot);
        return mat;
    }
};

template<class T>
class TAttr_RotateSpeed : public T
{
typedef T super;
private:
    float32 m_rspeed1;
    float32 m_rspeed2;

    istSerializeBlock(
        istSerializeBase(super)
        istSerialize(m_rspeed1)
        istSerialize(m_rspeed2)
        )

public:
    atmECallBlock(
        atmMethodBlock(
        atmECall(getRotateSpeed1)
        atmECall(setRotateSpeed1)
        atmECall(getRotateSpeed2)
        atmECall(setRotateSpeed2)
        )
        atmECallSuper(super)
    )

    wdmScope(
    void addDebugNodes(const wdmString &path)
    {
        T::addDebugNodes(path);
        wdmAddNode(path+"/m_rspeed1", &m_rspeed1, -3.0f, 3.0f);
        wdmAddNode(path+"/m_rspeed2", &m_rspeed2, -3.0f, 3.0f);
    }
    )

public:
    TAttr_RotateSpeed()
        : m_rspeed1(0.0f), m_rspeed2(0.0f)
    {}
    float32 getRotateSpeed1() const { return m_rspeed1; }
    float32 getRotateSpeed2() const { return m_rspeed2; }
    void setRotateSpeed1(float32 v) { m_rspeed1=v; }
    void setRotateSpeed2(float32 v) { m_rspeed2=v; }

    void updateRotate(float32 dt)
    {
        this->setRotate1(this->getRotate1()+getRotateSpeed1());
        this->setRotate2(this->getRotate2()+getRotateSpeed2());
    }
};

template<class T>
class TAttr_TransformMatrix : public T
{
typedef T super;
private:
    mat4 m_transform;

    istSerializeBlock(
        istSerializeBase(super)
        istSerialize(m_transform)
    )

public:
    atmECallBlock(
        atmMethodBlock(
            atmECall(getTransform)
            atmECall(setTransform)
            atmECall(updateTransformMatrix)
        )
        atmECallSuper(super)
    )
    wdmScope(void addDebugNodes(const wdmString &path) {
        T::addDebugNodes(path);
    })

public:
    const mat4& getTransform() const { return m_transform; }
    void setTransform(const mat4 &v) { m_transform=v; }
    void updateTransformMatrix()     { setTransform(super::computeMatrix()); }
};

template<class T>
class TAttr_TransformMatrixI : public T
{
typedef T super;
private:
    mat4 m_transform;
    mat4 m_itransform;

    istSerializeBlock(
        istSerializeBase(super)
        istSerialize(m_transform)
        istSerialize(m_itransform)
    )

public:
    atmECallBlock(
        atmMethodBlock(
            atmECall(getTransform)
            atmECall(setTransform)
            atmECall(getInverseTransform)
            atmECall(updateTransformMatrix)
        )
        atmECallSuper(super)
    )
    wdmScope(void addDebugNodes(const wdmString &path) {
        T::addDebugNodes(path);
    })

public:
    const mat4& getTransform() const        { return m_transform; }
    const mat4& getInverseTransform() const { return m_itransform; }

    void setTransform(const mat4 &v)
    {
        m_transform = v;
        m_itransform = glm::inverse(v);
    }

    void updateTransformMatrix()
    {
        setTransform(super::computeMatrix());
    }
};




class Attr_ParticleSet
{
private:
    vec4 m_diffuse_color;
    vec4 m_glow_color;
    PSET_RID m_psetid;

    istSerializeBlock(
        istSerialize(m_diffuse_color)
        istSerialize(m_glow_color)
        istSerialize(m_psetid)
    )

public:
    atmECallBlock(
        atmMethodBlock(
            atmECall(getDiffuseColor)
            atmECall(setDiffuseColor)
            atmECall(getGlowColor)
            atmECall(setGlowColor)
            atmECall(getModel)
            atmECall(setModel)
        )
    )

    wdmScope(
    void addDebugNodes(const wdmString &path)
    {
        wdmAddNode(path+"/m_diffuse_color", &m_diffuse_color, 0.0f, 1.0f);
        wdmAddNode(path+"/m_glow_color", &m_glow_color, 0.0f, 1.0f);
    }
    )

public:
    Attr_ParticleSet() : m_psetid(PSET_CUBE_SMALL)
    {}

    void setDiffuseColor(const vec4 &v) { m_diffuse_color=v; }
    void setGlowColor(const vec4 &v)    { m_glow_color=v; }
    void setModel(PSET_RID v)           { m_psetid=v; }
    const vec4& getDiffuseColor() const { return m_diffuse_color; }
    const vec4& getGlowColor() const    { return m_glow_color; }
    PSET_RID getModel() const           { return m_psetid; }

    void drawModel(const mat4 &trans)
    {
        PSetInstance inst;
        inst.diffuse = getDiffuseColor();
        inst.glow = getGlowColor();
        inst.flash = vec4();
        inst.elapsed = 0.0f;
        inst.appear_radius = 10000.0f;
        inst.translate = trans;
        atmGetSPHPass()->addPSetInstance(getModel(), inst);
    }
};


class Attr_Collision
{
private:
    CollisionHandle m_collision;
    EntityHandle m_owner_handle;

    istSerializeBlock(
        istSerialize(m_collision)
        istSerialize(m_owner_handle)
    )

public:
    atmECallBlock(
        atmMethodBlock(
            atmECall(getCollisionFlags)
            atmECall(setCollisionFlags)
            atmECall(getCollisionHandle)
            atmECall(setCollisionShape)
        )
    )
    wdmScope(void addDebugNodes(const wdmString &path) {})

public:
    Attr_Collision() : m_collision(0), m_owner_handle(0)
    {
    }

    ~Attr_Collision()
    {
        finalizeCollision();
    }

    void setCollisionFlags(int32 v)
    {
        if(CollisionEntity *ce=atmGetCollision(m_collision)) {
            ce->setFlags(v);
        }
    }

    uint32 getCollisionFlags() const
    {
        if(CollisionEntity *ce=atmGetCollision(m_collision)) {
            return ce->getFlags();
        }
        return 0;
    }

    void initializeCollision(EntityHandle h)
    {
        m_owner_handle = h;
    }

    void finalizeCollision()
    {
        if(m_collision!=0) {
            atmDeleteCollision(m_collision);
            m_collision = 0;
        }
    }

    void setCollisionShape(CollisionShapeID cs)
    {
        finalizeCollision();
        if(cs==CS_Null) {
            m_collision = 0;
            return;
        }
        CollisionEntity *ce = NULL;
        switch(cs) {
        case CS_Box:    ce = atmCreateCollision(CollisionBox);   break;
        case CS_Sphere: ce = atmCreateCollision(CollisionSphere);break;
        default: istAssert(false); return;
        }
        ce->setGObjHandle(m_owner_handle);
        m_collision = ce->getCollisionHandle();
    }

    CollisionHandle getCollisionHandle() const { return m_collision; }
    CollisionSphere& getCollisionSphere() {
        CollisionEntity *ce = atmGetCollision(m_collision);
        istAssert(ce!=nullptr && ce->getShape()==CS_Sphere);
        return *static_cast<CollisionSphere*>(ce);
    }
    CollisionBox& getCollisionBox() {
        CollisionEntity *ce = atmGetCollision(m_collision);
        istAssert(ce!=nullptr && ce->getShape()==CS_Box);
        return *static_cast<CollisionBox*>(ce);
    }

    void updateCollision(const mat4 &t)
    {
        if(CollisionEntity *ce = atmGetCollision(m_collision)) {
            switch(ce->getShape()) {
            case CS_Sphere:
                {
                    CollisionSphere &shape = *static_cast<CollisionSphere*>(ce);
                    vec3 pos = vec3(t * vec4(0.0f, 0.0f, 0.0f, 1.0f));
                    UpdateCollisionSphere(shape, pos, shape.pos_r.w);
                }
                break;
            case CS_Box:
                {
                    CollisionBox &shape = *static_cast<CollisionBox*>(ce);
                    UpdateCollisionBox(shape, t, vec3(shape.size));
                }
                break;
            }
        }
    }

    void updateCollisionAsSphere(const mat4 &t, float32 radius)
    {
        if(CollisionEntity *ce = atmGetCollision(m_collision)) {
            switch(ce->getShape()) {
            case CS_Sphere:
                {
                    vec3 pos = vec3(t * vec4(0.0f, 0.0f, 0.0f, 1.0f));
                    UpdateCollisionSphere(*static_cast<CollisionSphere*>(ce), pos, radius);
                }
                break;
            }
        }
    }

    void updateCollisionByParticleSet(PSET_RID psid, const mat4 &t, const vec3 &scale=vec3(1.0f))
    {
        if(CollisionEntity *ce = atmGetCollision(m_collision)) {
            switch(ce->getShape()) {
            case CS_Sphere:
                {
                    vec3 pos = vec3(t * vec4(0.0f, 0.0f, 0.0f, 1.0f));
                    float radius = atmGetRigidInfo(psid)->sphere_radius * scale.x;
                    UpdateCollisionSphere(*static_cast<CollisionSphere*>(ce), pos, radius);
                }
                break;
            case CS_Box:
                {
                    vec3 box_size = (vec3&)atmGetRigidInfo(psid)->box_size * scale;
                    UpdateCollisionBox(*static_cast<CollisionBox*>(ce), t, box_size);
                }
                break;
            }
        }
    }
};



struct CollideMessage;
struct DamageMessage;
struct DestroyMessage;
struct KillMessage;

class Attr_MessageHandler
{

    istSerializeBlock()

public:
    atmECallBlock(
        atmMethodBlock(
            atmECall(eventCollide)
            atmECall(eventFluid)
            atmECall(eventDamage)
            atmECall(eventDestroy)
            atmECall(eventKill)
        )
    )
    wdmScope(void addDebugNodes(const wdmString &path) {})

    virtual void eventCollide(const CollideMessage *m)  {}
    virtual void eventFluid(const FluidMessage *m)      {}
    virtual void eventDamage(const DamageMessage *m)    {}
    virtual void eventDestroy(const DestroyMessage *m)  {}
    virtual void eventKill(const KillMessage *m)        {}
};


// 流体を浴びた時血痕を残すエフェクトを実現する
class Attr_Bloodstain
{
private:
    // 血痕を残す頻度。流体がこの回数衝突したとき残す。
    static const uint32 bloodstain_frequency = 256;

    ist::raw_vector<BloodstainParticle> m_bloodstain;
    uint32 m_bloodstain_hitcount;

    istSerializeBlock(
        istSerialize(m_bloodstain)
        istSerialize(m_bloodstain_hitcount)
    )

public:
    atmECallBlock(
        atmMethodBlock(
            atmECall(addBloodstain)
        )
    )
    wdmScope(void addDebugNodes(const wdmString &path) {})

public:
    Attr_Bloodstain() : m_bloodstain_hitcount(0)
    {
        m_bloodstain.reserve(256);
    }

    void addBloodstain(const mat4 &imat, const vec4& pos)
    {
        if(!atmGetConfig()->show_bloodstain) { return; }

        if(++m_bloodstain_hitcount % bloodstain_frequency == 0) {
            BloodstainParticle tmp;
            tmp.position = imat * pos;
            tmp.lifetime = 1.0f;
            m_bloodstain.push_back(tmp);
        }
    }

    void updateBloodstain()
    {
        uint32 n = m_bloodstain.size();
        for(uint32 i=0; i<n; ++i) {
            BloodstainParticle &bsp = m_bloodstain[i];
            bsp.lifetime -= 0.002f;
        }
        m_bloodstain.erase(
            stl::remove_if(m_bloodstain.begin(), m_bloodstain.end(), BloodstainParticle_IsDead()),
            m_bloodstain.end());
    }

    uint32 getNumBloodstainParticles() const { return m_bloodstain.size(); }
    const BloodstainParticle* getBloodStainParticles() const { return m_bloodstain.empty() ? NULL : &m_bloodstain[0]; }
};

} // namespace atm
#endif // atm_Game_Entity_Attributes_h
