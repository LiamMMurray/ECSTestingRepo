#pragma once
#include "Memory.h"
class IPoolElement
{
    public:
        NMemory::index      m_parent_redirection_index;
        NMemory::index      m_redirection_index;
        NMemory::type_index m_pool_index;
        IPoolElement() : m_parent_redirection_index(0), m_redirection_index(0), m_pool_index(0)
        {}
        virtual ~IPoolElement();
};