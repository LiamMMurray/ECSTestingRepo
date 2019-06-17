#include "HandleManager.h"
#include "IComponent.h"

NHandleManager::NHandleManager(NMemory::NPools::RandomAccessPools& randomAccesspools, NMemory::byte* dynamic_memory) :
    component_random_access_pools(randomAccesspools),
    dynamic_memory(dynamic_memory),
    pool_count(TypeIndexFactory<IComponent>::GetTypeIndex<void>())
// TypeIdCreator<IComponent>::GetUniqueTypeId<void>() gets one past the last pool's index since this is the only place this
// function is called dynamically, and not statically.
{
        NComponentHandle::handleContext = this;
}

void NHandleManager::FreeComponent(NComponentHandle cHandle)
{
        NMemory::indices _adapter = {{cHandle.redirection_index}};
        Free(component_random_access_pools, cHandle.pool_index, _adapter);
}

void NHandleManager::ReleaseComponentHandle(NComponentHandle cHandle)
{
        NMemory::indices _adapter = {{cHandle.redirection_index}};
        ReleaseRedirectionIndices(component_random_access_pools, cHandle.pool_index, _adapter);
}

bool NHandleManager::IsActive(NComponentHandle cHandle)
{
        return component_random_access_pools.m_element_isactives[cHandle.pool_index][cHandle.redirection_index];
}

void NHandleManager::SetIsActive(NComponentHandle cHandle, bool isActive)
{
        component_random_access_pools.m_element_isactives[cHandle.pool_index][cHandle.redirection_index] = isActive;
}

NHandleManager* NComponentHandle::handleContext = 0;
void            NComponentHandle::Free()
{
        // TODO this MUST be deffered
        handleContext->FreeComponent(*this);
        handleContext->ReleaseComponentHandle(*this);
}

bool NComponentHandle::IsActive()
{
        return handleContext->IsActive(*this);
}

void NComponentHandle::SetIsActive(bool isActive)
{
        handleContext->SetIsActive(*this, isActive);
}

bool NComponentHandle::operator==(const NComponentHandle& other) const
{
        return other.pool_index == this->pool_index && other.redirection_index == this->redirection_index;
}
