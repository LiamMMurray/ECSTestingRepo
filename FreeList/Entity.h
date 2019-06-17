#pragma once
#include <unordered_map>
#include <vector>
#include "IPoolElement.h"
struct Entity : IPoolElement
{
        typedef std::unordered_map<NMemory::type_index, std::vector<NMemory::index>> entityComponentMap;

        static NMemory::index s_max_elements;

        static void           SSetMaxElements(NMemory::index max_elements);
        static NMemory::index SGetMaxElements();

        entityComponentMap m_OwnedComponents;

        std::vector<NMemory::index>& GetComponents(NMemory::type_index _type_index);
};
