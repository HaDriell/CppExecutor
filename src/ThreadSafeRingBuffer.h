#pragma once

#include <mutex>

template<class T, size_t Capacity>
class ThreadSafeRingBuffer
{
public:
    /// @brief Tries to push an item in the buffer
    /// @param item the item to push to the buffer
    /// @return true if the item could be pushed back
    bool push_back(const T& item)
    {
        std::scoped_lock<std::mutex> lock(m_Lock);

        size_t next = (m_Head +1) % Capacity;
        if (next != m_Tail)
        {
            m_Data[m_Head] = item;
            m_Head = next;
            return true;
        }

        return false;
    }

    bool pop_front(T& item)
    {
        std::scoped_lock<std::mutex> lock(m_Lock);

        if (m_Tail != m_Head)
        {
            item = m_Data[m_Tail];
            m_Tail = (m_Tail + 1) % Capacity;
            return true;
        }

        return false;
    }

private:
    T m_Data[Capacity];
    size_t m_Head = 0;
    size_t m_Tail = 0;
    std::mutex m_Lock;
};