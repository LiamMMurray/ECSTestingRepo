#pragma once
#include "HandleManager.h"

class IPoolElement
{
    public:
        EntityHandle    m_Owner;
        ComponentHandle m_Handle;
};