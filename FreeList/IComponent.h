#pragma once
#include "ECSTypes.h"
#include "HandleManager.h"
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


        EntityHandle                  GetOwner();
        NComponentHandle              GetHandle();
        void                          Enable();
        void                          Disable();
        void                          SetActive(bool state);
        bool                          IsEnabled();
        virtual const ComponentTypeId GetTypeId() const = 0;
};