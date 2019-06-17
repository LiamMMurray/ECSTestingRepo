#pragma once
#include "Pools.h"
#include "SpookyHashV2.h"
#include "Util.h"
#include <assert.h>
struct NHandleManager;

struct EntityHandle
{
        NMemory::index redirection_index;
};

struct ComponentHandle
{
    public:
        static NHandleManager* handleContext;
        friend struct NHandleManager;

        NMemory::type_index pool_index;
        NMemory::index      redirection_index;

        ComponentHandle(NMemory::type_index pool_index, NMemory::index redirection_index) :
            pool_index(pool_index),
            redirection_index(redirection_index)
        {}
        ComponentHandle() : pool_index(0), redirection_index(0)
        {}

    public:
        template <typename T>
        T* Get();

        void Free();

        bool IsActive();

        void SetIsActive(bool isActive);

        bool operator==(const ComponentHandle& other) const;
};

namespace std
{
        template <>
        struct hash<ComponentHandle>
        {
                size_t operator()(const ComponentHandle& h) const
                {
                        return SpookyHash::Hash64(&h, sizeof(h), 0);
                }
        };
} // namespace std

struct NHandleManager
{
        NMemory::index                      pool_count;
        NMemory::NPools::RandomAccessPools& component_random_access_pools;
        NMemory::NPools::RandomAccessPools& entity_random_access_pools;
        NMemory::byte*&                     dynamic_memory;
        NMemory::NPools::pool_descs         pool_descs;

        NHandleManager(NMemory::NPools::RandomAccessPools& componentRandomAccessPools,
                       NMemory::NPools::RandomAccessPools& entityRandomAccessPools,
                       NMemory::byte*                      dynamic_memory);

        template <typename T>
        T* GetComponent(ComponentHandle handle);

        template <typename T>
        ComponentHandle AddComponent();

		EntityHandle CreateEntity();

        void FreeComponent(ComponentHandle handle);

        void ReleaseComponentHandle(ComponentHandle cHandle);

        bool IsActive(ComponentHandle cHandle);

        void SetIsActive(ComponentHandle cHandle, bool isActive);
};

template <typename T>
inline T* NHandleManager::GetComponent(ComponentHandle cHandle)
{
        return reinterpret_cast<T*>(GetData(component_random_access_pools, cHandle.pool_index, cHandle.redirection_index));
}

template <typename T>
inline ComponentHandle NHandleManager::AddComponent()
{
        NMemory::type_index pool_index = T::SGetTypeIndex();
        if (component_random_access_pools.m_mem_starts.size() <= pool_index)
        {
                assert(dynamic_memory + sizeof(T) * T::SGetMaxElements() <= NMemory::GameMemory_Singleton::GameMemory_Max);
                InsertPool(component_random_access_pools, {sizeof(T), T::SGetMaxElements()}, dynamic_memory, pool_index);
        }
        NMemory::index redirection_index = Allocate(component_random_access_pools, pool_index);
        ComponentHandle componentHandle(pool_index, redirection_index);
        new (componentHandle.Get<T>()) T();
        return componentHandle;
}

template <typename T>
inline T* ComponentHandle::Get()
{
        return (handleContext->GetComponent<T>(*this));
}