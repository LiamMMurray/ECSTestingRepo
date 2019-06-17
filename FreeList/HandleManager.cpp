#include "HandleManager.h"
#include "IComponent.h"

NHandleManager::NHandleManager(NMemory::NPools::RandomAccessPools& componentRandomAccessPools,
                               NMemory::NPools::RandomAccessPools& entityRandomAccessPools,
                               NMemory::byte*                      dynamic_memory) :
    component_random_access_pools(componentRandomAccessPools),
	entity_random_access_pools(entityRandomAccessPools),
    dynamic_memory(dynamic_memory),
    pool_count(TypeIndexFactory<IComponent>::GetTypeIndex<void>())
// TypeIdCreator<IComponent>::GetUniqueTypeId<void>() gets one past the last pool's index since this is the only place this
// function is called dynamically, and not statically.
{
        ComponentHandle::handleContext = this;
}

void NHandleManager::FreeComponent(ComponentHandle cHandle)
{
        NMemory::indices _adapter = {{cHandle.redirection_index}};
        Free(component_random_access_pools, cHandle.pool_index, _adapter);
}

void NHandleManager::ReleaseComponentHandle(ComponentHandle cHandle)
{
        NMemory::indices _adapter = {{cHandle.redirection_index}};
        ReleaseRedirectionIndices(component_random_access_pools, cHandle.pool_index, _adapter);
}

bool NHandleManager::IsActive(ComponentHandle cHandle)
{
        return component_random_access_pools.m_element_isactives[cHandle.pool_index][cHandle.redirection_index];
}

void NHandleManager::SetIsActive(ComponentHandle cHandle, bool isActive)
{
        component_random_access_pools.m_element_isactives[cHandle.pool_index][cHandle.redirection_index] = isActive;
}

NHandleManager* ComponentHandle::handleContext = 0;
void            ComponentHandle::Free()
{
        // TODO this MUST be deffered
        handleContext->FreeComponent(*this);
        handleContext->ReleaseComponentHandle(*this);
}

bool ComponentHandle::IsActive()
{
        return handleContext->IsActive(*this);
}

void ComponentHandle::SetIsActive(bool isActive)
{
        handleContext->SetIsActive(*this, isActive);
}

bool ComponentHandle::operator==(const ComponentHandle& other) const
{
        return other.pool_index == this->pool_index && other.redirection_index == this->redirection_index;
}
