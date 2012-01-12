#include "stdafx.h"
#include "ist/ist.h"
#include "types.h"
#include "AtomicApplication.h"
#include "AtomicGame.h"
#include "Game/Message.h"
#include "Game/SPHManager.h"
#include "Game/Entity.h"
#include "Game/Collision.h"
#include "Game/World.h"
#include "Graphics/Renderer.h"
#include "EntityQuery.h"
#include "EntityClass.h"
#include "Util.h"

using namespace ist::i3d;


namespace atomic {



World::World()
: m_entity_set(NULL)
, m_collision_set(NULL)
, m_sph(NULL)
, m_frame(0)
{
    m_collision_set = istNew(CollisionSet)();
    m_entity_set    = istNew(EntitySet)();
    m_sph           = istNew(SPHManager)();

    m_task_update_world     = istNew(Task_UpdateAsync<World>)(this);
    m_task_update_collision = istNew(Task_UpdateAsync<CollisionSet>)(m_collision_set);
    m_task_update_entity    = istNew(Task_UpdateAsync<EntitySet>)(m_entity_set);
    m_task_update_sph       = istNew(Task_UpdateAsync<SPHManager>)(m_sph);

    const uvec2 &wsize = atomicGetWindowSize();
    m_camera.setAspect((float32)wsize.x/(float32)wsize.y);
}

World::~World()
{
    istSafeDelete(m_task_update_sph);
    istSafeDelete(m_task_update_entity);
    istSafeDelete(m_task_update_collision);
    istSafeDelete(m_task_update_world);

    istSafeDelete(m_sph);
    istSafeDelete(m_entity_set);
    istSafeDelete(m_collision_set);
}

void World::initialize()
{
    m_camera.setPosition(vec4(0.0f, 0.0f, 3.0f, 0.0f));
    m_camera.setZNear(0.01f);
    m_camera.setZFar(10.0f);

    m_sph->initialize();
}


void World::frameBegin()
{
    m_collision_set->frameBegin();
    m_sph->frameBegin();
    m_entity_set->frameBegin();
}

void World::update(float32 dt)
{
    ++m_frame;

    if(m_frame==1) {
        m_entity_set->createEntity<Level_Test>();
    }

    m_collision_set->update(dt);
    m_sph->update(dt);
    m_entity_set->update(dt);
}

void World::asyncupdateBegin(float32 dt)
{
    m_task_update_world->setArg(dt);
    m_task_update_collision->setArg(dt);
    m_task_update_sph->setArg(dt);
    m_task_update_entity->setArg(dt);

    Task *tasks[] = {
        m_task_update_world,
        m_task_update_collision,
        m_task_update_sph,
        m_task_update_entity,
    };
    TaskScheduler::addTask(tasks, _countof(tasks));
}

void World::asyncupdateEnd()
{
    Task *tasks[] = {
        m_task_update_world,
        m_task_update_collision,
        m_task_update_sph,
        m_task_update_entity,
    };
    TaskScheduler::waitFor(tasks, _countof(tasks));
}

void World::asyncupdate(float32 dt)
{
    //mat4 rot = glm::rotate(mat4(), 0.05f, vec3(0.0f, 1.0f, 0.0f));
    //m_camera.setPosition(rot * m_camera.getPosition());
    //m_camera.setAspect(atomicGetWindowAspectRatio());
}

void World::draw() const
{
    m_sph->draw();
    m_entity_set->draw();

    DirectionalLight dl;
    dl.setDirection(glm::normalize(vec4(1.0f, -1.0f, -0.5f, 0.0f)));
    dl.setDiffuse(vec4(0.3f, 0.3f, 0.3f, 1.0f));
    dl.setAmbient(vec4(0.0f, 0.0f, 0.0f, 0.0f));
    atomicGetDirectionalLights()->addInstance(dl);
}

void World::frameEnd()
{
    m_entity_set->frameEnd();
    m_sph->frameEnd();
    m_collision_set->frameEnd();
}


} // namespace atomic
