#pragma once
#include "HandleManager.h"
#include "Memory.h"

class IPoolElement
{
    public:
        NEntityHandle    m_Owner;
        NComponentHandle m_Handle;
};

class IComponent : IPoolElement
{
    public:
        IComponent()
        {}

        virtual ~IComponent()
        {}

        virtual const NMemory::type_index GetTypeId() const = 0;
};