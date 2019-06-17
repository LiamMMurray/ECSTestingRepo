
#include <assert.h>
#include <algorithm>
#include <iterator>
#include <list>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include "Component.h"
#include "MemoryLeakDetection.h"
#include "Pools.h"
class TransformComponent : public Component<TransformComponent>
{
    public:
		TransformComponent()
		{
                a = 0;
                b = 0;
                c = 0;
                d = 0;
		}

        std::vector<int> av;
        float            a;
        float            b;
        float            c;
        float            d;
};

class PhysicsComponent : public Component<PhysicsComponent>
{
    public:
        PhysicsComponent()
        {
                a000 = 0;
                a001 = 0;
                b000 = 0;
                b001 = 0;
                c000 = 0;
                c001 = 0;
                d000 = 0;
                d001 = 0;
        }

        float a000;
        float a001;
        float b000;
        float b001;
        float c000;
        float c001;
        float d000;
        float d001;
};

#include "HandleManager.h"

using namespace NMemory;
using namespace NMemory::NPools;

index rand_range(index min, index max)
{
        return static_cast<index>(rand()) % max + min;
}

index rand_exclude_range(std::vector<index>& indices, index min, index max)
{
        index out = rand_range(min, max);
        for (auto i : indices)
        {
                if (out == i)
                        return rand_exclude_range(indices, min, max);
        }
        return out;
}

int _main()
{
        memsize        alloc_size_request      = 80000000;
        NMemory::byte* debug_address           = 0;
        GameMemory_Singleton::GameMemory_Start = NMemory::ReserveGameMemory(alloc_size_request);
        debug_address                          = GameMemory_Singleton::GameMemory_Start;
        assert(GameMemory_Singleton::GameMemory_Start != 0);
        GameMemory_Singleton::GameMemory_Curr = GameMemory_Singleton::GameMemory_Start;
        GameMemory_Singleton::GameMemory_Max  = GameMemory_Singleton::GameMemory_Start + alloc_size_request;

        RandomAccessPools componentRandomAccessPools;
        RandomAccessPools entityRandomAccessPools;
        HandleManager handleManager(componentRandomAccessPools, entityRandomAccessPools, GameMemory_Singleton::GameMemory_Curr);

        TransformComponent::SSetMaxElements(9000);
        std::vector<ComponentHandle>              cHandles;
        std::vector<EntityHandle>                 eHandles;
        dynamic_bitset                            isActives;
        std::unordered_map<ComponentHandle, bool> isActivesMap001;
        std::unordered_map<ComponentHandle, bool> isActivesMap002;

        for (size_t i = 0; i < 1000; i++)
        {
                eHandles.push_back(handleManager.CreateEntity());
        }

        for (size_t i = 0; i < 1000; i++)
        {
                cHandles.push_back(handleManager.AddComponent<TransformComponent>(eHandles[0]));
                if (rand() % 2 == 1)
                {
                        cHandles[i].SetIsActive(false);
                        isActives.push_back(false);
                        assert(cHandles[i].IsActive() == false);
                        isActivesMap001[cHandles[i]] = false;
                }
                else
                {
                        isActives.push_back(true);
                        assert(cHandles[i].IsActive() == true);
                        isActivesMap001[cHandles[i]] = true;
                }
                cHandles[i].Get<TransformComponent>()->a = i;
                cHandles[i].Get<TransformComponent>()->b = 1000 + i;
                cHandles[i].Get<TransformComponent>()->c = 10000 + i;
                cHandles[i].Get<TransformComponent>()->d = 100000 + i;
        }
        size_t idx_offset = 0;
        size_t idx_end    = cHandles.size() + 1000;
        for (size_t i = cHandles.size(); i < idx_end; i++)
        {
                cHandles.push_back(handleManager.AddComponent<PhysicsComponent>(eHandles[idx_offset]));
                cHandles[i].Get<PhysicsComponent>()->a000 = idx_offset;
                cHandles[i].Get<PhysicsComponent>()->a001 = 2000 + idx_offset;
                cHandles[i].Get<PhysicsComponent>()->b000 = 20000 + idx_offset;
                cHandles[i].Get<PhysicsComponent>()->b001 = 200000 + idx_offset;
                cHandles[i].Get<PhysicsComponent>()->c000 = 999999;
                cHandles[i].Get<PhysicsComponent>()->c001 = 999999;
                cHandles[i].Get<PhysicsComponent>()->d000 = 777777;
                cHandles[i].Get<PhysicsComponent>()->d001 = 777777;
                idx_offset++;
        }
        index              max_handle_index = cHandles.back().redirection_index;
        std::vector<index> deleted_handles;
        for (size_t i = 0; i < 100; i++)
        {
                index idx = rand_exclude_range(deleted_handles, 0, max_handle_index);
                deleted_handles.push_back(idx);
                cHandles[idx].Free();
        }
        for (size_t i = 0; i < 1000; i++)
        {
                if (std::find(deleted_handles.begin(), deleted_handles.end(), i) != deleted_handles.end())
                {
                        auto* null_ptr = ComponentHandle(0, i).Get<TransformComponent>();
                        assert(null_ptr == 0);
                }
                else
                {
                        auto* valid_ptr = ComponentHandle(0, i).Get<TransformComponent>();
                        assert(valid_ptr != 0);
                }
        }
        for (size_t i = 0; i < 100; i++)
        {
                handleManager.AddComponent<TransformComponent>(eHandles[i]);
        }
        for (size_t i = 0; i < cHandles.size(); i++)
        {
                isActivesMap002[cHandles[i]] = cHandles[i].IsActive();
        }

        int incrementer = 0;
        for (auto kv : isActivesMap001)
        {
                // this handle is not deleted
                if (std::find(deleted_handles.begin(), deleted_handles.end(), kv.first.redirection_index) ==
                    deleted_handles.end())
                {
                        assert(kv.second == isActivesMap002[kv.first]);
                        incrementer++;
                }
        }

        return 0;
}


int main()
{
        _main();

        NMemory::FreeGameMemory();

        _CrtDumpMemoryLeaks();

        return 0;
}
