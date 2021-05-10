#pragma once
#include "types.h"
#include "hardware/memory.hpp"

template <typename T> class VectorList
{
    public:
        // constructor
        VectorList()
        {
            Data = nullptr;
            Size = 0;
            Count = 0;
            CurrentIndex = 0;
            SizeInBytes = 0;
        }

        ~VectorList() { delete Data; Data = nullptr; }

        void Push(T data)
        {
            // new array
            T* temp = new T[Size + 1];

            if (Data != nullptr)
            {
                // copy old array to new array
                for (size_t i = 0; i < Size; i++)
                {
                    temp[i] = Data[i];
                }
            }

            // create new array
            if (Data != nullptr) { delete Data; }
            Data = new T[Size + 1];

            // copy old items to new array
            for (size_t i = 0; i < Size; i++) { Data[i] = temp[i]; }

            // copy new item new new array
            Data[Size] = data;  

            // increment size and count
            SizeInBytes += sizeof(data);
            Size += 1;
            CurrentIndex++;
            Count++;
        }

        void Push(T data, uint32_t index)
        {
            
        }

        void Pop()
        {
            
        }

        T Get(uint32_t index) { return Data[index]; }
        uint32_t GetCount() { return Count; }
        uint32_t GetSizeInBytes() { return SizeInBytes; }
        T* GetPointer() { return Data; }

    private:
        T* Data;
        uint32_t SizeInBytes;
        uint32_t CurrentIndex;
        uint32_t Size;
        uint32_t Count;
};