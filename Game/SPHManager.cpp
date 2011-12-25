#include "stdafx.h"
#include "ist/ist.h"
#include "types.h"
#include "AtomicApplication.h"
#include "Graphics/Renderer.h"
#include "Game/Message.h"
#include "Game/AtomicGame.h"
#include "Game/World.h"
#include "Game/SPHManager.h"

namespace atomic {




SPHManager::SPHManager()
{
}

SPHManager::~SPHManager()
{
}


void SPHManager::initialize()
{
}


void SPHManager::updateBegin( float32 dt )
{
    m_spheres.clear();
    m_boxes.clear();

    m_pgravity.clear();
}

void SPHManager::update(float32 dt)
{
    const thrust::host_vector<sphFluidMessage> &message = SPHGetFluidMessage();
}

void SPHManager::asyncupdate(float32 dt)
{
    SPHUpdateGravity(m_pgravity);
    SPHUpdateRigids(m_spheres, m_boxes);
    SPHUpdateFluid();
}

void SPHManager::addRigidSphere(const sphRigidSphere &s)
{
    m_spheres.push_back(s);
}

void SPHManager::addRigidBox(const sphRigidBox &s)
{
    m_boxes.push_back(s);
}

void SPHManager::addPointGravity(const sphForcePointGravity &v)
{
    m_pgravity.push_back(v);
}

void SPHManager::draw() const
{
}


} // namespace atomic
