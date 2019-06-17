#include "Memory.h"
#define WIN_32_LEAN_AND_MEAN
#include <Windows.h>
#include <assert.h>
#include <malloc.h>
namespace NMemory
{
        byte* GameMemory_Singleton::GameMemory_Start = 0;
        byte* GameMemory_Singleton::GameMemory_Curr  = 0;
        byte* GameMemory_Singleton::GameMemory_Max   = 0;

        byte* ReserveGameMemory(memsize memsize)
        {
                return static_cast<byte*>(VirtualAlloc(0, memsize, MEM_COMMIT, PAGE_READWRITE));
                // return static_cast<byte*>(malloc(memsize));
        }
        void FreeGameMemory()
        {
                int error = VirtualFree(GameMemory_Singleton::GameMemory_Start, 0, MEM_RELEASE);

                if (error == 0)
                {
                        auto error = GetLastError();
                        assert(false);
                }
                // free(GameMemory_Singleton::GameMemory_Start);
        }
}; // namespace NMemory