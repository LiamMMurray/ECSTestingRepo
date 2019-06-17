#pragma once
#include <unordered_map>
#include <vector>
#include "IPoolElement.h"
struct Entity : IPoolElement
{
        typedef std::unordered_map<NMemory::type_index, std::vector<NMemory::index>> entityComponentMap;
        entityComponentMap*                                                          m_OwnedComponents;
        std::vector<NMemory::index>&                                                 GetComponents(NMemory::type_index _type_index)
        {
                return (*m_OwnedComponents)[_type_index];
        }
        void Initialize()
        {
                m_OwnedComponents = new entityComponentMap();
        }
        void ShutDown()
        {
                delete m_OwnedComponents;
        }
};
