#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

class IndexedBitset
{
public:
    IndexedBitset() = default;
    IndexedBitset(size_t aBitCount) { create(aBitCount); }
    void create(size_t aBitCount)
    {
        m_Data.clear();
        if (aBitCount == 0)
            return;
        do
        {
            aBitCount = (aBitCount + 63) / 64;
            m_Data.emplace_back(aBitCount);
        } while (aBitCount != 1);
    }
    void set(size_t aBit)
    {
        for (std::vector<uint64_t>& sLayer : m_Data)
        {
            size_t sWord = aBit / 64;
            size_t sMask = 1ull << (aBit % 64);
            sLayer[sWord] |= sMask;
            aBit = sWord;
        }
    }
    void clear(size_t aBit)
    {
        for (std::vector<uint64_t>& sLayer : m_Data)
        {
            size_t sWord = aBit / 64;
            size_t sMask = 1ull << (aBit % 64);
            sLayer[sWord] &= ~sMask;
            if (sLayer[sWord] != 0)
                break;
            aBit = sWord;
        }
    }
    size_t lowest() const // must not be empty!
    {
        assert(!empty());
        size_t sBit = 0;
        auto sItr = m_Data.cend();
        do
        {
            --sItr;
            assert((*sItr)[sBit] != 0);
            sBit = sBit * 64 + __builtin_ctzll((*sItr)[sBit]);
            
        } while (sItr != m_Data.cbegin());
        return sBit;
    }
    bool empty() const
    {
        return m_Data.empty() || m_Data.back()[0] == 0;
    }
private:

    std::vector<std::vector<uint64_t> > m_Data;

};