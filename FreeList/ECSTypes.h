#pragma once
#include <stdint.h>
#include <functional>
#include "Handle.h"
#include "Memory.h"

template <typename T>
struct TypeIndex
{
        NMemory::type_index m_Data;

        operator NMemory::type_index() const
        {
                return m_Data;
        }

        TypeIndex() : m_Data(0)
        {}

        TypeIndex(uint32_t data) : m_Data(data)
        {}

        TypeIndex operator++(int)
        {
                TypeIndex temp(*this);
                m_Data++;
                return temp;
        }
        bool operator==(const TypeIndex<T>& other) const
        {
                return this->m_Data == other.m_Data;
        }
        bool operator!=(const TypeIndex<T>& other) const
        {
                return this->m_Data != other.m_Data;
        }
};

template <typename T>
class std::hash<TypeIndex<T>>
{
    public:
        size_t operator()(const TypeIndex<T> id) const
        {
                return hash<uint32_t>()(id.m_Data);
        }
};

class IComponent;
class IEntity;
class ISystem;

typedef Handle<IComponent>    ComponentHandle;
typedef Handle<IEntity>       EntityHandle;
typedef TypeIndex<IComponent> ComponentTypeId;
typedef TypeIndex<IEntity>    EntityTypeId;
typedef TypeIndex<ISystem>    SystemTypeId;