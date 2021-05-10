#pragma once
#include "types.h"
#include "hardware/memory.hpp"

namespace System
{

    // C++, your mom is a bitch
    template<class Type> class List
    {
    private:
        Type** list = nullptr;
    public:
        int Count = 0;
        void Add(Type element)
        {
            Type* e = new Type;
            *e = element;
            if (Count == 0 || list == nullptr)
            {
                list = new Type*[++Count];
                list[0] = e;
            }
            else
            {
                Type** temp = new Type*[++Count];
                mem_copy((uint8_t*)list, (uint8_t*)temp, size(list));
                delete list;
                list = temp;
                list[Count - 1] = e;
            }
        }
        void Remove(Type element)
        {
            if (list == nullptr) { debug_throw_message(MSG_TYPE_ERROR, "Given list was empty"); return; }
            int index = -1;
            for (int i = 0; i < Count; i++)
            {
                if (*list[i] == element)
                {
                    index = i;
                    break;
                }
            }
            if (index == -1) { debug_throw_message(MSG_TYPE_ERROR, "Element was not found in the given list"); return; }
            delete list[index];
            if (Count - 1 == 0)
            {
                delete list;
                list = nullptr;
            }
            else
            {
                Type** temp = new Type*[Count - 1];
                for (int i = 0, s = 0; i < Count; i++)
                    if (i != index)
                    {
                        temp[s] = list[i];
                        s++;
                    }
                --Count;
                delete list;
                list = temp;
            }
        }
        void RemoveAt(uint32_t index)
        {
            if (list == nullptr) { debug_throw_message(MSG_TYPE_ERROR, "Given list was empty"); return; }
            if (index < 0 || index > Count) { debug_throw_message(MSG_TYPE_ERROR, "Index outside the array's bounds"); return; }
            delete list[index];
            if (Count - 1 == 0)
            {
                delete list;
                list = nullptr;
            }
            else
            {
                Type** temp = new Type*[Count - 1];
                for (int i = 0, s = 0; i < Count; i++)
                    if (i != index)
                    {
                        temp[s] = list[i];
                        s++;
                    }
                --Count;
                delete list;
                list = temp;
            }

        }
        Type* Get(uint32_t index)
        {
            if (list == nullptr) { debug_throw_message(MSG_TYPE_ERROR, "Given list was empty"); return nullptr; }
            if (index < 0 || index > Count) { debug_throw_message(MSG_TYPE_ERROR, "Index outside the array's bounds"); return nullptr; }
            
            return list[index];
        }
        void Set(uint32_t index, Type data)
        {
            if (list == nullptr) { debug_throw_message(MSG_TYPE_ERROR, "Given list was empty"); return; }
            if (index < 0 || index > Count) { debug_throw_message(MSG_TYPE_ERROR, "Index outside the array's bounds"); return; }

            delete list[index];
            Type* e = new Type;
            *e = data;
            list[index] = e;
        }

        ~List()
        {
            for (int i = 0; i < Count; i++)
                delete list[i];
            delete list;
        }
    };
}