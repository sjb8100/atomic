﻿#include "stdafx.h"
#include "types.h"
#include "AtomicGame.h"
#include "Graphics/ResourceManager.h"
#include "EntityModule.h"
#include "EntityQuery.h"
#include "CollisionModule.h"
#include "World.h"
#include "Task.h"

#ifdef atm_enable_strict_handle_check
    #define atmStrictHandleCheck(h) if(!isValidHandle(h)) { istAssert("invalid entity handle\n"); }
#else
    #define atmStrictHandleCheck(h)
#endif

namespace atm {


EntityCreator* GetEntityCreatorTable( EntityClassID entity_classid )
{
    static EntityCreator s_table_player[EC_Player_End & 0x1FF];
    static EntityCreator s_table_enemy[EC_Enemy_End & 0x1FF];
    static EntityCreator s_table_obstacle[EC_Obstacle_End & 0x1FF];
    static EntityCreator s_table_level[EC_Level_End & 0x1FF];
    static EntityCreator s_table_vfx[EC_VFX_End & 0x1FF];
    static EntityCreator *s_table_list[ECA_End] = {
        NULL,
        s_table_player,
        s_table_enemy,
        s_table_obstacle,
        s_table_level,
        s_table_vfx,
    };
    return s_table_list[(entity_classid & 0xE00) >> 9];
}

void AddEntityCreator(EntityClassID entity_classid, EntityCreator creator)
{
    GetEntityCreatorTable(entity_classid)[entity_classid & 0x1FF] = creator;
}

IEntity* CreateEntity( EntityClassID entity_classid )
{
    return GetEntityCreatorTable(entity_classid)[entity_classid & 0x1FF]();
}



atmExportClass(IEntity);

IEntity::IEntity()
    : m_ehandle(atmGetWorld() ? atmGetEntityModule()->getGeneratedHandle() : 0)
{
}


atmExportClass(EntityModule);

EntityModule::EntityModule()
{
}

EntityModule::~EntityModule()
{
    Entities &entities = m_entities;
    uint32 s = entities.size();
    for(uint32 k=0; k<s; ++k) {
        istSafeDelete(entities[k]);
    }
    entities.clear();
    m_vacants.clear();
    m_all.clear();
}

void EntityModule::initialize()
{
}

void EntityModule::frameBegin()
{
}

void EntityModule::update( float32 dt )
{
    // update
    for(uint32 i=0; i<m_all.size(); ++i) {
        if(IEntity *entity = getEntity(m_all[i])) {
            entity->update(dt);
        }
        else {
            m_all[i] = 0;
        }
    }

    // erase invalid handles
    m_all.erase(stl::remove(m_all.begin(), m_all.end(), 0), m_all.end());

    m_vacants.insert(m_vacants.end(), m_dead_prev.begin(), m_dead_prev.end());
    m_dead_prev = m_dead;
    m_dead.clear();


    // asyncupdate
    atmDbgLockSyncMethods();
    ist::parallel_for(
        ist::size_range(size_t(0), m_all.size(), 32),
        [&](const ist::size_range &r) {
            for(size_t i=r.begin(); i!=r.end(); ++i) {
                if(IEntity *e=getEntity(m_all[i])) {
                    e->asyncupdate(dt);
                }
            }
        });
    atmDbgUnlockSyncMethods();
}

void EntityModule::asyncupdate(float32 dt)
{
}

void EntityModule::draw()
{
    uint32 s = m_entities.size();
    for(uint32 k=0; k<s; ++k) {
        IEntity *entity = m_entities[k];
        if(entity) { entity->draw(); }
    }
}

void EntityModule::frameEnd()
{
}


IEntity* EntityModule::getEntity( EntityHandle h )
{
    if(h==0) { return nullptr; }
    uint32 cid = EntityGetClassID(h);
    uint32 iid = EntityGetIndex(h);

    Entities &entities = m_entities;
    if(iid >= entities.size()) {
        return nullptr;
    }
    return entities[iid];
}

void EntityModule::deleteEntity( EntityHandle h )
{
    atmDbgAssertSyncLock();
    uint32 cid = EntityGetClassID(h);
    uint32 iid = EntityGetIndex(h);
    Entities &entities = m_entities;
    Handles &vacants = m_vacants;
    entities[iid]->finalize();
    istSafeRelease(entities[iid]);
    m_dead.push_back(EntityGetIndex(h));
}

void EntityModule::generateHandle(EntityClassID classid)
{
    atmDbgAssertSyncLock();
    Entities &entities = m_entities;
    Handles &vacant = m_vacants;
    EntityHandle h = 0;
    if(!vacant.empty()) {
        h = vacant.back();
        vacant.pop_back();
    }
    else {
        h = entities.size();
        entities.push_back(nullptr); // reserve
    }
    h = EntityCreateHandle(classid, h);
    m_tmp_handle = h;
}

EntityHandle EntityModule::getGeneratedHandle()
{
    return m_tmp_handle;
}

IEntity* EntityModule::createEntity( EntityClassID classid )
{
    generateHandle(classid);
    IEntity *e = CreateEntity(classid);
    m_entities[EntityGetIndex(e->getHandle())] = e;
    m_all.push_back(e->getHandle());
    e->initialize();
    return e;
}


void EntityModule::handleEntitiesQuery( EntitiesQueryContext &ctx )
{
#ifdef atm_enable_WebGL
    mat4 trans;
    vec4 color = vec4(vec3(1.0f), 0.1f);
    vec3 size;
#endif // atm_enable_WebGL

    CollisionHandle ch;
    uint32 num_entities = m_all.size();
    for(uint32 i=0; i<num_entities; ++i) {
        EntityHandle handle = m_all[i];
        IEntity *e = getEntity(handle);
        if(e) {
            if(!atmQuery(e, getCollisionHandle, ch)) { continue; }
            CollisionEntity *ce = atmGetCollision(ch);
            if(ce) {
#ifdef atm_enable_WebGL
                atmQuery(e, getTransformMatrix, trans);
                switch(ce->getShapeType()) {
                case CS_Sphere:
                    size = vec3(static_cast<CollisionSphere*>(ce)->pos_r.w*0.8f);
                    break;
                case CS_Box:
                    size = vec3(static_cast<CollisionBox*>(ce)->size);
                    break;
                default:
                    size = vec3();
                    break;
                }

                switch(EntityGetCategory(handle)) {
                case ECA_Player:   color=vec4(0.3f, 0.3f, 1.0f, 0.25f); break;
                case ECA_Enemy:    color=vec4(1.0f, 0.3f, 0.3f, 0.25f); break;
                case ECA_Obstacle: color=vec4(0.4f, 0.4f, 0.4f, 0.25f); break;
                default:           color=vec4(); break;
                }

                ctx.id.push_back(handle);
                ctx.trans.push_back(trans);
                ctx.size.push_back(size);
                ctx.color.push_back(color);
#else // atm_enable_WebGL
                const BoundingBox &bb = ce->bb;
                vec4 bb_size = bb.ur - bb.bl;
                vec4 bb_pos = (bb.ur + bb.bl) * 0.5f;
                ctx.id.push_back(handle);
                ctx.type.push_back( EntityGetCategory(e->getHandle()) );
                ctx.size.push_back( vec2(bb_size) );
                ctx.pos.push_back( vec2(bb_pos) );
#endif // atm_enable_WebGL
            }
        }
    }
}


} // namespace atm