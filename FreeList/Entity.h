#pragma once
#include "IEntity.h"
#include "Util.h"

template <typename T>
class Entity : public IEntity
{
    private:
        static const NMemory::type_index s_type_index;

    public:
        static const NMemory::type_index SGetTypeIndex();
        const NMemory::type_index GetTypeIndex() const;
};
template <typename T>
const NMemory::type_index Entity<T>::s_type_index = TypeIndexFactory<IEntity>::GetTypeIndex<T>();

template <typename T>
inline const NMemory::type_index Entity<T>::GetTypeIndex() const
{
        return s_type_index;
}

template <typename T>
inline const NMemory::type_index Entity<T>::SGetTypeIndex()
{
        return s_type_index;
}