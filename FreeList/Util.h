#pragma once
#include "Memory.h"
template <typename T>
class TypeIdCreator
{
        static NMemory::type_index m_Count;
    public:
        template <typename U>
        static NMemory::type_index GetUniqueTypeId()
        {
                static const NMemory::type_index increment_once_per_secondary_type{m_Count++};
                return increment_once_per_secondary_type;
        }
};
template <typename T>
NMemory::type_index TypeIdCreator<T>::m_Count{0};
