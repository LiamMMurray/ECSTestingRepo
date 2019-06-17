
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
        float a;
        float b;
        float c;
        float d;
};

class PhysicsComponent : public Component<PhysicsComponent>
{
    public:
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
        memsize alloc_size_request             = 8000000;
        GameMemory_Singleton::GameMemory_Start = NMemory::ReserveGameMemory(alloc_size_request);
        assert(GameMemory_Singleton::GameMemory_Start != 0);
        GameMemory_Singleton::GameMemory_Curr = GameMemory_Singleton::GameMemory_Start;
        GameMemory_Singleton::GameMemory_Max  = GameMemory_Singleton::GameMemory_Start + alloc_size_request;

        RandomAccessPools componentRandomAccessPools;
        RandomAccessPools entityRandomAccessPools;
        NHandleManager    handleManager(
            componentRandomAccessPools, entityRandomAccessPools, GameMemory_Singleton::GameMemory_Curr);

        TransformComponent::SSetMaxElements(9000);
        std::vector<ComponentHandle>              handles;
        dynamic_bitset                            isActives;
        std::unordered_map<ComponentHandle, bool> isActivesMap001;
        std::unordered_map<ComponentHandle, bool> isActivesMap002;
        for (size_t i = 0; i < 1000; i++)
        {
                handles.push_back(handleManager.AddComponent<TransformComponent>());
                if (rand() % 2 == 1)
                {
                        handles[i].SetIsActive(false);
                        isActives.push_back(false);
                        assert(handles[i].IsActive() == false);
                        isActivesMap001[handles[i]] = false;
                }
                else
                {
                        isActives.push_back(true);
                        assert(handles[i].IsActive() == true);
                        isActivesMap001[handles[i]] = true;
                }
                handles[i].Get<TransformComponent>()->a = i;
                handles[i].Get<TransformComponent>()->b = 1000 + i;
                handles[i].Get<TransformComponent>()->c = 10000 + i;
                handles[i].Get<TransformComponent>()->d = 100000 + i;
        }
        size_t idx_offset = 0;
        size_t idx_end    = handles.size() + 1000;
        for (size_t i = handles.size(); i < idx_end; i++)
        {
                handles.push_back(handleManager.AddComponent<PhysicsComponent>());
                handles[i].Get<PhysicsComponent>()->a000 = idx_offset;
                handles[i].Get<PhysicsComponent>()->a001 = 2000 + idx_offset;
                handles[i].Get<PhysicsComponent>()->b000 = 20000 + idx_offset;
                handles[i].Get<PhysicsComponent>()->b001 = 200000 + idx_offset;
                handles[i].Get<PhysicsComponent>()->c000 = 999999;
                handles[i].Get<PhysicsComponent>()->c001 = 999999;
                handles[i].Get<PhysicsComponent>()->d000 = 777777;
                handles[i].Get<PhysicsComponent>()->d001 = 777777;
                idx_offset++;
        }
        index              max_handle_index = handles.back().redirection_index;
        std::vector<index> deleted_handles;
        for (size_t i = 0; i < 100; i++)
        {
                index idx = rand_exclude_range(deleted_handles, 0, max_handle_index);
                deleted_handles.push_back(idx);
                handles[idx].Free();
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
                handleManager.AddComponent<TransformComponent>();
        }
        for (size_t i = 0; i < handles.size(); i++)
        {

                isActivesMap002[handles[i]] = handles[i].IsActive();
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

        _CrtDumpMemoryLeaks();

        return 0;
}
