#pragma once
#include "Pools.h"
#include "Util.h"
struct NHandleManager;

struct NComponentHandle
{
    public:
        static NHandleManager* handleContext;
        friend struct NHandleManager;

        NMemory::type_index pool_index;
        NMemory::index      redirection_index;

        NComponentHandle(NMemory::type_index pool_index, NMemory::index redirection_index) :
            pool_index(pool_index),
            redirection_index(redirection_index)
        {}
        NComponentHandle() : pool_index(0), redirection_index(0)
        {}

    public:
        template <typename T>
        T* Get();

        void Free();
};

struct fuck_you_visual_studio
{
        int pool_indx;
        int redirection_index;
};

struct NHandleManager
{
        NMemory::index                      pool_count;
        NMemory::NPools::RandomAccessPools& component_random_access_pools;
        NMemory::NPools::RandomAccessPools  entity_random_access_pools;
        NMemory::byte*&                     dynamic_memory;
        NMemory::NPools::pool_descs         pool_descs;

        NHandleManager(NMemory::NPools::RandomAccessPools& randomAccesspools, NMemory::byte* dynamic_memory);

        template <typename T>
        T* GetComponent(NComponentHandle handle);

        template <typename T>
        NComponentHandle AddComponent();

        void FreeComponent(NComponentHandle handle);

        void ReleaseComponentHandle(NComponentHandle handle);
};

template <typename T>
inline T* NHandleManager::GetComponent(NComponentHandle handle)
{
        return reinterpret_cast<T*>(GetData(component_random_access_pools, handle.pool_index, handle.redirection_index));
}

template <typename T>
inline NComponentHandle NHandleManager::AddComponent()
{
        ComponentTypeId pool_index = T::SGetTypeId();
        if (component_random_access_pools.m_mem_starts.size() <= pool_index)
        {
                assert(dynamic_memory + sizeof(T) * T::GetMaxElements() <= GameMemory_Singleton::GameMemory_Max);
                InsertPool(component_random_access_pools, {sizeof(T), T::GetMaxElements()}, dynamic_memory, pool_index);
        }
        NMemory::index redirection_index = Allocate(component_random_access_pools, pool_index.m_Data);
        return NComponentHandle(pool_index, redirection_index);
}

template <typename T>
inline T* NComponentHandle::Get()
{
        return (handleContext->GetComponent<T>(*this));
}