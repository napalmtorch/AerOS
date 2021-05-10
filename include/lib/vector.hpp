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
            delete temp;
        }

        void Push(T data, uint32_t index)
        {
            
        }

        void Pop(uint32_t index)
        {
            // check if index is valid
            if (index >= GetCount()) { System::KernelIO::ThrowError("Vector list index out of bounds"); return; }
            if (Data == nullptr) { System::KernelIO::ThrowError("Tried to pop item from null vector list"); return; }

            // temporary list
            uint32_t new_size = Size - 1;
            uint32_t i = 0;
            T* temp = new T[new_size];

            // delete item
            Data[index] = NULL;

            // store data from old list
            uint32_t new_i = 0;
            for (i = 0; i < new_size; i++)
            {
                if (i != index) { temp[new_i] = Data[i]; new_i++; }
            }

            // create new list
            if (Data != nullptr) { delete Data; }
            Data = new T[new_size];

            // copy data to new list
            for (i = 0; i < new_size; i++) { Data[i] = temp[i]; }

            // delete temporary data
            delete temp;
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