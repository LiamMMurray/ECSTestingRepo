#pragma once
#include "Memory.h"
namespace NMemory
{
        namespace NPools
        {
                struct Pool_Desc
                {
                        memsize element_size;
                        index   element_capacity;
                };
                typedef std::vector<Pool_Desc> pool_descs;


                struct ForwardAccessPools
                {
                        std::vector<byte*>   m_mem_starts;
                        std::vector<index>   m_element_counts;
                        std::vector<memsize> m_elment_byte_sizes;
                        std::vector<index>   m_element_capacities;
                };

                void AppendPools(ForwardAccessPools& pools, const pool_descs& _pool_descs, byte*& dynamic_mem);

                struct RandomAccessPools : public ForwardAccessPools
                {
                        std::vector<indices>              m_redirection_indices;
                        std::vector<index_priority_queue> m_free_redirection_indices;
                        // std::vector<deletion_accumulators>      m_deletion_accumulators;
                };

                void  AppendPools(RandomAccessPools& forward_access_pools, const pool_descs& _pool_descs, byte*& dynamic_mem);

                void InsertPool(RandomAccessPools& pools, const Pool_Desc& _pool_desc, byte*& dynamic_mem, index pool_index);


                byte* GetData(RandomAccessPools& component_random_access_pools, index pool_index, index data_index);
                void  Free(RandomAccessPools& component_random_access_pools, index pool_index, indices& index_buffer_indexes);
                index Allocate(RandomAccessPools& component_random_access_pools, index pool_index);
                void  ReleaseRedirectionIndices(RandomAccessPools& component_random_access_pools,
                                                index              pool_index,
                                                indices&           redirection_indices);


                // intializes pool data
                // mem_start gets modified to be new mem start
                // void PushBack(RandomAccessPools& random_access_pools, const pool_descs& _pool_descs, byte*& mem_start);

                // intializes pool data
                // dymamic_mem gets used and returns new dynamic_mem

        } // namespace NPools
} // namespace NMemory