#pragma once
#include "IComponent.h"
#include "Memory.h"
#include "Util.h"

template <typename T>
class Component : public IComponent
{
    private:
        static const NMemory::type_index m_TypeId;
        static NMemory::index            m_default_max_elements;

    public:
        Component();
        void                             Init();
        static const NMemory::type_index SGetTypeId();
        const NMemory::type_index        GetTypeId() const;
        static void                      SetMaxElements(NMemory::index max_elements)
        {
                m_default_max_elements = max_elements;
        }
        static NMemory::index GetMaxElements()
        {
                return m_default_max_elements;
        }
};
template <class T>
const NMemory::type_index Component<T>::m_TypeId = TypeIdCreator<IComponent>::GetUniqueTypeId<T>();
template <class T>
NMemory::index Component<T>::m_default_max_elements = 5000;

template <typename T>
inline Component<T>::Component()
{
        static_assert(std::is_base_of<Component<T>, T>::value, "Components must derive from Component<T>");
}

template <typename T>
inline void Component<T>::Init()
{
        static_cast<T*>(this)->Init();
}

template <typename T>
inline const NMemory::type_index Component<T>::SGetTypeId()
{
        static_assert(std::is_base_of<Component<T>, T>::value, "Components must follow derive from Component<T>");
        return m_TypeId;
}

template <typename T>
inline const NMemory::type_index Component<T>::GetTypeId() const
{
        return m_TypeId;
}