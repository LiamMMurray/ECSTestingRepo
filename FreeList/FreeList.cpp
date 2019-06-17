//// FreeList.cpp : This file contains the 'main' function. Program execution
//// begins and ends there.
////
//
//#include <stdint.h>
////#include <iostream>
//#include <assert.h>
//#include <stdlib.h>
//#include <string>
//#include <vector>
//#include "MemoryLeakDetection.h"
//class IComponent
//{};
//
//enum HANDLEFLAG
//{
//        INACTIVE = 0b00000001,
//        DESTROY  = 0b00000010
//};
//struct HandleData
//{
//        uint32_t m_DeletionAccumulator : 24;
//        uint32_t m_Flags : 8;
//
//        bool IsValid()
//        {
//                return (m_Flags & HANDLEFLAG::INACTIVE) == 0;
//        }
//        void SetInvalid()
//        {
//                SetFlags(m_Flags | HANDLEFLAG::INACTIVE);
//        }
//        void SetFlags(uint8_t flags)
//        {
//                m_Flags = flags;
//        }
//};
//template <typename T>
//struct Handle
//{
//        Handle() : m_Data(0)
//        {}
//        inline void Initialize()
//        {
//                m_Data = 0;
//        }
//        Handle(uint32_t id, uint32_t version = 0U, uint8_t flags = 0U) :
//            m_Id(id),
//            m_DeletionAccumulator(version),
//            m_Flags(flags)
//        {}
//        union
//        {
//                uint64_t m_Data;
//                struct
//                {
//                        uint32_t m_Id;
//                        union
//                        {
//                                HandleData m_HandleData;
//                                struct
//                                {
//                                        uint32_t m_DeletionAccumulator : 24;
//                                        uint32_t m_Flags : 8;
//                                };
//                        };
//                };
//        };
//        bool operator==(const Handle<T>& other) const
//        {
//                return this->m_Data == other.m_Data;
//        }
//        bool IsValid()
//        {
//                return (m_Flags & HANDLEFLAG::INACTIVE) == 0;
//        }
//        void SetInvalid()
//        {
//                SetFlags(m_Flags | HANDLEFLAG::INACTIVE);
//        }
//        void SetFlags(uint8_t flags)
//        {
//                m_Flags = flags;
//        }
//};
//
//class IAllocator
//{
//    public:
//        virtual void*    GetMemory()          = 0;
//        virtual uint32_t GetCapacity()        = 0;
//        virtual uint32_t GetElementByteSize() = 0;
//};
//
//template <typename MainType, typename SubType, uint32_t MAX_ELEMENTS>
//class FreeList : public IAllocator
//{
//        union element
//        {
//                struct
//                {
//                        SubType e;
//                };
//                uint32_t next;
//        };
//
//    public:
//        FreeList()
//        {}
//        ~FreeList()
//        {}
//
//    private:
//        uint32_t m_free_start = 0;
//        element*    m_pool           = 0;
//        HandleData* m_pool_meta_data = 0;
//
//    public:
//        void* GetMemory()
//        {
//                return m_pool;
//        }
//        uint32_t GetCapacity()
//        {
//                return MAX_ELEMENTS;
//        }
//        uint32_t GetElementByteSize()
//        {
//                return sizeof(element);
//		}
//        void Initialize()
//        {
//                m_pool           = (element*)DBG_MALLOC(sizeof(element) * MAX_ELEMENTS);
//                m_pool_meta_data = (HandleData*)DBG_MALLOC(sizeof(HandleData) * MAX_ELEMENTS);
//
//                for (uint32_t i = 0; i < MAX_ELEMENTS; i++)
//                {
//                        m_pool[i].next = i + 1;
//                }
//                for (uint32_t i = 0; i < MAX_ELEMENTS; i++)
//                {
//                        m_pool_meta_data[i] = {};
//                }
//                m_pool[MAX_ELEMENTS - 1].next = -1;
//        }
//        void Shutdown()
//        {
//                free(m_pool_meta_data);
//                free(m_pool);
//        }
//
//        Handle<MainType> Allocate()
//        {
//                Handle<MainType> out;
//                out.m_Id                  = m_free_start;
//                out.m_DeletionAccumulator = m_pool_meta_data[m_free_start].m_DeletionAccumulator;
//                assert(out.m_Id != -1); // free list out of memory
//                m_free_start = m_pool[out.m_Id].next;
//                return out;
//        }
//        void Free(Handle<MainType>& handle)
//        {
//                m_pool[handle.m_Id].next = m_free_start;
//                m_pool_meta_data[handle.m_Id].m_DeletionAccumulator++;
//                m_free_start = handle.m_Id;
//        }
//        SubType* operator[](Handle<MainType> handle)
//        {
//                assert(handle.m_DeletionAccumulator ==
//                       m_pool_meta_data[handle.m_Id].m_DeletionAccumulator); // this object was deleted
//                // if (handle.m_DeletionAccumulator != m_pool_meta_data[handle.m_Id].m_DeletionAccumulator)
//                //		return nullptr
//                // I dont like this... going to work around it if we can... if we have need of this operator than i will see
//                // about changing this
//                return &(m_pool[handle.m_Id].e);
//        }
//};
//
//template <typename MainType>
//class IContainer
//{
//    public:
//        virtual void             Shutdown()                          = 0;
//        virtual void             Initialize()                        = 0;
//        virtual Handle<MainType> Allocate()                          = 0;
//        virtual void             Free(Handle<MainType>& handle)      = 0;
//        virtual MainType*        operator[](Handle<MainType> handle) = 0;
//
//		inline iterator begin()
//        {
//                return iterator(this->m_Chunks.begin(), this->m_Chunks.end());
//        }
//        inline iterator end()
//        {
//                return iterator(this->m_Chunks.end(), this->m_Chunks.end());
//        }
//};
//
//template <typename MainType, typename SubType, uint32_t MAX_ELEMENTS>
//class Container : public IContainer<MainType>, public FreeList<MainType, SubType, MAX_ELEMENTS>
//{
//    private:
//        typedef FreeList<MainType, SubType, MAX_ELEMENTS> Allocator;
//
//    public:
//        void Initialize()
//        {
//                Allocator::Initialize();
//        }
//        Handle<MainType> Allocate()
//        {
//                return Allocator::Allocate();
//        }
//        void Free(Handle<MainType>& handle)
//        {
//                Allocator::Free(handle);
//        };
//        MainType* operator[](Handle<MainType> handle)
//        {
//                return static_cast<MainType*>(Allocator::operator[](handle));
//        };
//        void Shutdown()
//        {
//                Allocator::Shutdown();
//        }
//};
//
//class IClass
//{
//        virtual void abstractFunction() = 0;
//        float        a;
//        float        b;
//        float        c;
//        // sizeof = 12
//};
//class a : public IClass
//{
//        void abstractFunction()
//        {}
//        float d;
//        // sizeof = 16
//    public:
//        float my_guy;
//        int   tasty;
//        void  Initialize()
//        {
//                my_guy = 42.1;
//                tasty  = 5;
//                d      = 1.2;
//        }
//};
//
//class b : public IClass
//{
//        void abstractFunction()
//        {}
//        float e;
//        float f;
//        // sizeof = 20;
//};
//
//int main()
//{
//        std::vector<IContainer<IClass>*> containers(10);
//        FreeList<IClass, class a, 9000>  fr_lst;
//        Container<IClass, class a, 9000> _cntr;
//        containers[5] = new Container<IClass, a, 9000>();
//        containers[5]->Initialize();
//        containers[5]->Allocate();
//
//        fr_lst.Initialize();
//        _cntr.Initialize();
//
//        auto _a = _cntr[5];
//        auto _b = _cntr.Allocate();
//        auto _c = _cntr.Allocate();
//
//        fr_lst.Shutdown();
//        _cntr.Shutdown();
//        containers[5]->Shutdown();
//        _CrtDumpMemoryLeaks();
//        return 0;
//}
//
//// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
//// Debug program: F5 or Debug > Start Debugging menu
//
//// Tips for Getting Started:
////   1. Use the Solution Explorer window to add/manage files
////   2. Use the Team Explorer window to connect to source control
////   3. Use the Output window to see build output and other messages
////   4. Use the Error List window to view errors
////   5. Go to Project > Add New Item to create new code files, or Project >
////   Add Existing Item to add existing code files to the project
////   6. In the future, to open this project again, go to File > Open > Project
////   and select the .sln file
