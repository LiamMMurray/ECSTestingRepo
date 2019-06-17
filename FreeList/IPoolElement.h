#pragma once
#include "HandleManager.h"

class IPoolElement
{
    public:
        NEntityHandle    m_Owner;
        NComponentHandle m_Handle;
};