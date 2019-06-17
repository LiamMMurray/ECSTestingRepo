#pragma once
#include "Pools.h"
#include "SpookyHashV2.h"
#include "Util.h"
struct NHandleManager;

struct NEntityHandle
{
        NMemory::type_index entity_index;
};

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

        bool IsActive();

        void SetIsActive(bool isActive);

        bool operator==(const NComponentHandle& other) const;
};

namespace std
{
        template <>
        struct hash<NComponentHandle>
        {
                size_t operator()(const NComponentHandle& h) const
                {
                        return SpookyHash::Hash64(&h, sizeof(h), 0);
                }
        };
} // namespace std

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

        void ReleaseComponentHandle(NComponentHandle cHandle);

        bool IsActive(NComponentHandle cHandle);

        void SetIsActive(NComponentHandle cHandle, bool isActive);
};

template <typename T>
inline T* NHandleManager::GetComponent(NComponentHandle cHandle)
{
        return reinterpret_cast<T*>(GetData(component_random_access_pools, cHandle.pool_index, cHandle.redirection_index));
}

template <typename T>
inline NComponentHandle NHandleManager::AddComponent()
{
        NMemory::type_index pool_index = T::SGetTypeIndex();
        if (component_random_access_pools.m_mem_starts.size() <= pool_index)
        {
                assert(dynamic_memory + sizeof(T) * T::SGetMaxElements() <= GameMemory_Singleton::GameMemory_Max);
                InsertPool(component_random_access_pools, {sizeof(T), T::SGetMaxElements()}, dynamic_memory, pool_index);
        }
        NMemory::index redirection_index = Allocate(component_random_access_pools, pool_index);
        return NComponentHandle(pool_index, redirection_index);
}

template <typename T>
inline T* NComponentHandle::Get()
{
        return (handleContext->GetComponent<T>(*this));
}