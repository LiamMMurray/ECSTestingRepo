
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
#include "Range.h"
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


class RagdollComponent : public Component<RagdollComponent>
{};


class AComponent : public Component<AComponent>
{};
class BComponent : public Component<BComponent>
{};
class CComponent : public Component<CComponent>
{};
class DComponent : public Component<DComponent>
{};
struct declar
{
        int a = AComponent::SGetTypeIndex();
        int b = BComponent::SGetTypeIndex();
        int c = CComponent::SGetTypeIndex();
        int d = DComponent::SGetTypeIndex();
};

#include "HandleManager.h"

using namespace NMemory;
using namespace NMemory::NPools;

index rand_range(index min, index max)
{
        index range = max + 1 - min;
        return static_cast<index>(rand()) % range + min;
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

#include <bitset>
#include <type_traits>
#include <vector>

template <typename... Ts>
typename std::enable_if<sizeof...(Ts) == 0>::type GetTypeFilter(std::bitset<NMemory::filter_size>& filterFlags)
{}

template <typename T, typename... Ts>
void GetTypeFilter(std::bitset<filter_size>& filterFlags)
{
        filterFlags[T::SGetTypeIndex()] = true;
        return GetTypeFilter<Ts...>(filterFlags);
}
struct FHandleManager
{

        std::unordered_map<std::bitset<filter_size>, std::vector<NMemory::type_index>> cached_filter_searches;
        std::bitset<filter_size>                                                       dirty_filters;

        void Search(std::bitset<filter_size>& search_filter, std::vector<std::bitset<filter_size>>& group_filters)
        {
                std::vector<NMemory::type_index> out;
                for (int i = 0; i < group_filters.size(); i++)
                {
                        auto _b = group_filters[i];
                        _b &= search_filter;
                        _b ^= search_filter;
                        if (_b.none())
                                out.push_back(i);
                }
                cached_filter_searches[search_filter] = out;
        }
        bool AlreadyExists(std::bitset<filter_size> _filter)
        {
                return false;
        }
        void FAddComponentGroup(std::bitset<filter_size> _filter)
        {
                if (AlreadyExists(_filter)) {}
                else
                {
                        dirty_filters |= _filter;
                }
        }
};
int _main()
{
        declar                   d;
        int                      TypeCount = TypeIndexFactory<IComponent>::GetTypeIndex<void>();
        std::bitset<filter_size> filter;
        assert(filter_size >= TypeCount);

        std::vector<std::bitset<filter_size>> rand_filters(1000);
        for (auto& b : rand_filters)
        {
                for (int i = 0; i < filter_size; i++)
                {
                        b[i] = rand() % 2;
                }
        }
        std::vector<std::bitset<filter_size>>    rand_search_filters(1000);
        std::vector<std::vector<NMemory::index>> rand_search_filters_flagged_bits;
        for (auto& b : rand_search_filters)
        {
                size_t                      num_filter_flags = rand_range(2, 19);
                std::vector<NMemory::index> flagged_bits;
                for (size_t i = 0; i < num_filter_flags; i++)
                {
                        auto idx = rand_exclude_range(flagged_bits, 0, filter_size - 1);
                        flagged_bits.push_back(idx);
                        b[idx] = 1;
                }
                rand_search_filters_flagged_bits.push_back(flagged_bits);
        }
        FHandleManager hm;
        for (auto& b : rand_search_filters)
        {
                hm.Search(b, rand_filters);
        }
        return 0;
}


int main()
{
        _main();

        _CrtDumpMemoryLeaks();

        return 0;
}
