#pragma once
#include <assert.h>
#include "Pools.h"
#include "SpookyHashV2.h"
#include "Util.h"

struct NHandleManager;
struct Entity;
class IPoolElement;

struct EntityHandle
{
    public:
        static NHandleManager* handleContext;
        friend struct NHandleManager;

        NMemory::index redirection_index;

        EntityHandle(NMemory::index);

    public:
        Entity* Get();

        void Free();

        bool IsActive();

        void SetIsActive(bool isActive);

        bool operator==(const EntityHandle& other) const;
};

struct ComponentHandle
{
    public:
        static NHandleManager* handleContext;
        friend struct NHandleManager;

        NMemory::type_index pool_index;
        NMemory::index      redirection_index;

        ComponentHandle(NMemory::type_index pool_index, NMemory::index redirection_index);
        ComponentHandle();

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
        template <>
        struct hash<EntityHandle>
        {
                size_t operator()(const EntityHandle& h) const
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
        ~NHandleManager();
        template <typename T>
        T* GetComponent(ComponentHandle handle);

        Entity* GetEntity(EntityHandle handle);

        template <typename T>
        ComponentHandle AddComponent(EntityHandle parentHandle);

        EntityHandle CreateEntity(EntityHandle parentHandle = -1);

        void FreeComponent(ComponentHandle handle);

        void ReleaseComponentHandle(ComponentHandle handle);

        void ReleaseEntityHandle(EntityHandle handle);

        bool IsActive(ComponentHandle handle);

        void SetIsActive(ComponentHandle handle, bool isActive);

        void FreeEntity(EntityHandle handle);

        bool IsActive(EntityHandle handle);

        void SetIsActive(EntityHandle handle, bool isActive);

		void ShutDown();
};

template <typename T>
inline T* NHandleManager::GetComponent(ComponentHandle cHandle)
{
        return reinterpret_cast<T*>(GetData(component_random_access_pools, cHandle.pool_index, cHandle.redirection_index));
}
#include "Entity.h"
template <typename T>
inline ComponentHandle NHandleManager::AddComponent(EntityHandle parentHandle)
{
        NMemory::type_index pool_index = T::SGetTypeIndex();
        if (component_random_access_pools.m_mem_starts.size() <= pool_index)
        {
                if (dynamic_memory + sizeof(T) * T::SGetMaxElements() >= NMemory::GameMemory_Singleton::GameMemory_Max)
                        assert(false);
                InsertPool(component_random_access_pools, {sizeof(T), T::SGetMaxElements()}, dynamic_memory, pool_index);
        }
        auto            allocation = Allocate(component_random_access_pools, pool_index);
        ComponentHandle componentHandle(pool_index, allocation.redirection_idx);
        T*              objectPtr = reinterpret_cast<T*>(allocation.objectPtr);
        new (objectPtr) T();
        objectPtr->m_pool_index               = componentHandle.pool_index;
        objectPtr->m_redirection_index        = componentHandle.redirection_index;
        objectPtr->m_parent_redirection_index = parentHandle.redirection_index;

        Entity*        entities_mem_start = reinterpret_cast<Entity*>(entity_random_access_pools.m_mem_starts[0]);
        NMemory::index parent_index       = entity_random_access_pools.m_redirection_indices[0][parentHandle.redirection_index];
        Entity*        parent_mem         = entities_mem_start + parent_index;

        parent_mem->m_OwnedComponents.emplace(componentHandle.pool_index, componentHandle.redirection_index);
        return componentHandle;
}

template <typename T>
inline T* ComponentHandle::Get()
{
        return (handleContext->GetComponent<T>(*this));
}