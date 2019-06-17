#include "HandleManager.h"

NHandleManager::NHandleManager(NMemory::NPools::RandomAccessPools& randomAccesspools, NMemory::byte* dynamic_memory) :
    component_random_access_pools(randomAccesspools),
    dynamic_memory(dynamic_memory),
    pool_count(TypeIdCreator<IComponent>::GetUniqueTypeId<void>())
{
        NComponentHandle::handleContext = this;
}

void NHandleManager::FreeComponent(NComponentHandle handle)
{
        NMemory::indices _adapter = {{handle.redirection_index}};
        Free(component_random_access_pools, handle.pool_index, _adapter);
}

void NHandleManager::ReleaseComponentHandle(NComponentHandle handle)
{
        NMemory::indices _adapter = {{handle.redirection_index}};
        ReleaseRedirectionIndices(component_random_access_pools, handle.pool_index, _adapter);
}

NHandleManager* NComponentHandle::handleContext = 0;
void            NComponentHandle::Free()
{
        // TODO this MUST be deffered
        handleContext->FreeComponent(*this);
        handleContext->ReleaseComponentHandle(*this);
}
