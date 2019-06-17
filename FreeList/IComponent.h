#pragma once
#include "IPoolElement.h"

class IComponent : IPoolElement
{
    public:
        IComponent()
        {}

        virtual ~IComponent()
        {}

        virtual const NMemory::type_index GetTypeIndex() const = 0;
};