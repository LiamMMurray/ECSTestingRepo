#pragma once
#include "IComponent.h"
#include "Memory.h"
#include "Util.h"

template <typename T>
class Component : public IComponent
{
    private:
        static const NMemory::type_index s_type_index;
        static NMemory::index            s_max_elements;

    public:
        Component();
        void                             Init();
        const NMemory::type_index        GetTypeIndex() const;
        static const NMemory::type_index SGetTypeIndex();
        static void                      SSetMaxElements(NMemory::index max_elements);
        static NMemory::index            SGetMaxElements();
};
template <class T>
const NMemory::type_index Component<T>::s_type_index = TypeIndexFactory<IComponent>::GetTypeIndex<T>();
template <class T>
NMemory::index Component<T>::s_max_elements = 5000;

template <typename T>
inline Component<T>::Component()
{
}

template <typename T>
inline void Component<T>::Init()
{
        static_cast<T*>(this)->Init();
}

template <typename T>
inline const NMemory::type_index Component<T>::SGetTypeIndex()
{
        return s_type_index;
}

template <typename T>
inline const NMemory::type_index Component<T>::GetTypeIndex() const
{
        return s_type_index;
}

template <typename T>
inline void Component<T>::SSetMaxElements(NMemory::index max_elements)
{
        s_max_elements = max_elements;
}

template <typename T>
inline NMemory::index Component<T>::SGetMaxElements()
{
        return s_max_elements;
}