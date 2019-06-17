#pragma once
#include "ECSTypes.h"
#include "HandleManager.h"

class IComponent
{

    public:
        bool            m_Enabled = true;
        EntityHandle    m_Owner;
        NComponentHandle m_Handle;

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