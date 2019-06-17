#pragma once
#include <queue>
#include <vector>

namespace NMemory
{
        typedef char                                                                byte;
        typedef size_t                                                              memsize;
        typedef size_t                                                              index;
        typedef size_t                                                              type_index;
        typedef std::vector<index>                                                  indices;
        typedef size_t                                                              deletion_accumulator;
        typedef std::vector<deletion_accumulator>                                   deletion_accumulators;
        typedef std::vector<index>                                                  delete_requests;
        typedef std::priority_queue<index, std::vector<index>, std::greater<index>> index_priority_queue;
        typedef size_t                                                              entity_index;

        struct GameMemory_Singleton
        {
            public:
                static byte* GameMemory_Start;
                static byte* GameMemory_Curr;
                static byte* GameMemory_Max;
        };


        // Allocates memory on the system level
        byte* ReserveGameMemory(memsize memsize);
        void FreeGameMemory();
} // namespace NMemory
