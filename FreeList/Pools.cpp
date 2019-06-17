#include "Pools.h"
#include <assert.h>
#include "HandleManager.h"
#include "IComponent.h"
namespace NMemory
{
        namespace NPools
        {
                // intializes pool data
                // mem_start gets modified to be new mem start
                void AppendPools(ForwardAccessPools& pools, const pool_descs& _pool_descs, byte*& dynamic_mem)
                {
                        index pool_count      = pools.m_element_capacities.size();
                        index pool_desc_count = _pool_descs.size();
                        index new_pool_count  = pool_count + pool_desc_count;

                        // reserve memory in vectors
                        for (index i = pool_count; i < new_pool_count; i++)
                        {
                                pools.m_element_capacities.resize(new_pool_count);
                                pools.m_elment_byte_sizes.resize(new_pool_count);
                                pools.m_mem_starts.resize(new_pool_count);
                        }
                        // iterate and set data
                        for (index i = pool_count; i < new_pool_count; i++)
                        {
                                pools.m_element_capacities[i] = _pool_descs[i].element_capacity;
                                pools.m_elment_byte_sizes[i]  = _pool_descs[i].element_size;
                                pools.m_mem_starts[i]         = dynamic_mem;
                                dynamic_mem += _pool_descs[i].element_capacity * _pool_descs[i].element_size;
                        }
                }

                // intializes pool data
                // mem_start gets modified to be new mem start
                void AppendPools(RandomAccessPools& pools, const pool_descs& _pool_descs, byte*& dynamic_mem)
                {
                        index pool_count      = pools.m_element_capacities.size();
                        index pool_desc_count = _pool_descs.size();
                        index new_pool_count  = pool_count + pool_desc_count;

                        /// resize all element vectors
                        for (index i = pool_count; i < new_pool_count; i++)
                        {
                                pools.m_element_capacities.resize(new_pool_count);
                                pools.m_redirection_indices.resize(new_pool_count);
                                pools.m_free_redirection_indices.resize(new_pool_count);
                                pools.m_elment_byte_sizes.resize(new_pool_count);
                                pools.m_element_counts.resize(new_pool_count);
                                pools.m_mem_starts.resize(new_pool_count);
                                pools.m_element_isactives.resize(new_pool_count);
                        }

                        index _i_descs = 0;
                        index _i_pool  = pool_count;
                        for (; _i_pool < new_pool_count; _i_pool++, _i_descs++)
                        {
                                pools.m_element_capacities[_i_pool] = _pool_descs[_i_descs].element_capacity;
                                pools.m_elment_byte_sizes[_i_pool]  = _pool_descs[_i_descs].element_size;
                                pools.m_mem_starts[_i_pool]         = dynamic_mem;
                                dynamic_mem += _pool_descs[_i_descs].element_capacity * _pool_descs[_i_descs].element_size;

                                index index_count = _pool_descs[_i_descs].element_capacity;
                                pools.m_redirection_indices[_i_pool].resize(index_count);
                                pools.m_element_isactives[_i_pool].resize(index_count);
                                for (index j = 0; j < index_count; j++)
                                {
                                        pools.m_free_redirection_indices[_i_pool].push(j);
                                }
                        }
                }

                void InsertPool(RandomAccessPools& pools, const Pool_Desc& _pool_desc, byte*& dynamic_mem, index pool_index)
                {
                        index pool_count = pools.m_element_capacities.size();

                        if (pool_count <= pool_index)
                        {
                                pool_descs _pool_descs;
                                _pool_descs.resize(pool_index - (pool_count - 1));
                                for (auto& _ps : _pool_descs)
                                {
                                        _ps = {};
                                }
                                _pool_descs[pool_index - (pool_count - 1) - 1].element_capacity = _pool_desc.element_capacity;
                                _pool_descs[pool_index - (pool_count - 1) - 1].element_size     = _pool_desc.element_size;
                                AppendPools(pools, _pool_descs, dynamic_mem);
                        }
                        else
                        {
                                pools.m_element_capacities[pool_index] = _pool_desc.element_capacity;
                                pools.m_elment_byte_sizes[pool_index]  = _pool_desc.element_size;
                                pools.m_mem_starts[pool_index]         = dynamic_mem;
                                dynamic_mem += _pool_desc.element_capacity * _pool_desc.element_size;

                                index index_count = _pool_desc.element_capacity;
                                pools.m_redirection_indices[pool_index].resize(index_count);
                                pools.m_element_isactives[pool_index].resize(index_count);
                                for (index j = 0; j < index_count; j++)
                                {
                                        pools.m_free_redirection_indices[pool_index].push(j);
                                }
                        }
                }

                byte* GetData(RandomAccessPools& component_random_access_pools, index pool_index, index index_buffer_index)
                {
                        index element_index =
                            component_random_access_pools.m_redirection_indices[pool_index][index_buffer_index];
                        if (element_index == -1)
                                return 0;

                        memsize element_size = component_random_access_pools.m_elment_byte_sizes[pool_index];
                        byte*   mem_start    = component_random_access_pools.m_mem_starts[pool_index];

                        return mem_start + element_index * element_size;
                }
                void Free(RandomAccessPools& component_random_access_pools,
                          index              pool_index,
                          indices&           deleted_redirection_indices)
                {
                        memsize element_size       = component_random_access_pools.m_elment_byte_sizes[pool_index];
                        byte*   mem_start          = component_random_access_pools.m_mem_starts[pool_index];
                        index   last_element_index = component_random_access_pools.m_element_counts[pool_index] - 1;

                        for (index i = 0; i < deleted_redirection_indices.size(); i++)
                        {
                                // this  function performs a sorted pool's function copying over the memory of the deleted
                                // element with that of the last element.
                                // It also includes a priority queue of indices to maintain persistent identity and random order
                                // access.
                                index deleted_redirection_index = deleted_redirection_indices[i];

                                index deleted_element_index =
                                    component_random_access_pools.m_redirection_indices[pool_index][deleted_redirection_index];

                                byte* last_element_mem    = mem_start + last_element_index * element_size;
                                byte* deleted_element_mem = mem_start + deleted_element_index * element_size;

                                // copy over the deleted element's data with last element's data
                                memcpy(deleted_element_mem, last_element_mem, element_size);

                                // get the handle of the last element that is copying over the deleted element
                                IComponent*      icomponent          = reinterpret_cast<IComponent*>(last_element_mem);
                                NComponentHandle last_element_handle = icomponent->m_Handle;

                                // set the last element's redirection value to the deleted element's redirection value
                                component_random_access_pools
                                    .m_redirection_indices[pool_index][last_element_handle.redirection_index] =
                                    component_random_access_pools.m_redirection_indices[pool_index][deleted_redirection_index];

                                icomponent->m_Handle.pool_index =
                                    component_random_access_pools.m_redirection_indices[pool_index][deleted_redirection_index];

                                component_random_access_pools.m_redirection_indices[pool_index][deleted_redirection_index] = -1;

                                last_element_index--;
                        }
                        component_random_access_pools.m_element_counts[pool_index] = last_element_index + 1;
                }

                // get redirection_index for allocation
                index Allocate(RandomAccessPools& component_random_access_pools, index pool_index)
                {
                        index next_free = component_random_access_pools.m_free_redirection_indices[pool_index].top();
                        component_random_access_pools.m_free_redirection_indices[pool_index].pop();
                        component_random_access_pools.m_redirection_indices[pool_index][next_free] =
                            component_random_access_pools.m_element_counts[pool_index];

                        index   element_index       = component_random_access_pools.m_element_counts[pool_index];
                        byte*   elemement_mem_start = component_random_access_pools.m_mem_starts[pool_index];
                        memsize elemement_size      = component_random_access_pools.m_elment_byte_sizes[pool_index];
                        byte*   element_mem         = elemement_mem_start + element_index * elemement_size;
                        reinterpret_cast<IComponent*>(element_mem)->m_Handle = NComponentHandle(pool_index, next_free);

                        component_random_access_pools.m_element_isactives[pool_index][next_free] = true;

                        component_random_access_pools.m_element_counts[pool_index]++;
                        assert(component_random_access_pools.m_element_counts[pool_index] <=
                               component_random_access_pools.m_element_capacities[pool_index]);

                        return next_free;
                }

                void ReleaseRedirectionIndices(RandomAccessPools& component_random_access_pools,
                                               index              pool_index,
                                               indices&           redirection_indices)
                {
                        for (index i = 0; i < redirection_indices.size(); i++)
                        {
                                if (component_random_access_pools.m_redirection_indices[pool_index][redirection_indices[i]] ==
                                    -1)
                                        component_random_access_pools.m_free_redirection_indices[pool_index].push(
                                            redirection_indices[i]);
                        }
                }

                void Clear(RandomAccessPools& component_random_access_pools, index pool_index)
                {
                        index element_capacity = component_random_access_pools.m_element_capacities[pool_index];
                        component_random_access_pools.m_element_counts[pool_index] = 0;
                        component_random_access_pools.m_free_redirection_indices[pool_index].empty();
                        for (index i = 0; i < element_capacity; i++)
                        {
                                component_random_access_pools.m_free_redirection_indices[pool_index].push(i);
                        }
                }
        } // namespace NPools
} // namespace NMemory