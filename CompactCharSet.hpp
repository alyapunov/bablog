#pragma once

#include <cstdint>
#include <cstdlib>
#include <utility>

#if 1
class CompactCharSet7
{
public:
    CompactCharSet7() { }
    CompactCharSet7(const unsigned char* aValues) { build(aValues); }
    void build(const unsigned char* aSortedValues)
    {
        m_Index = 0;
        for (size_t i = 0; i < 7; i++)
            m_Index |= static_cast<uint64_t>(aSortedValues[i]) << (i * 8);
        m_Index = ~m_Index;
    }
    std::pair<bool, size_t> find(unsigned char c) const
    {
        //uint64_t mask = 0x0101010101010101ull;
        uint64_t mask = 0x0001010101010101ull;
        uint64_t a = mask * c;
        a ^= m_Index;
        a = (a & 0x80808080808080) & ((a & 0x7f7f7f7f7f7f7f) + mask);
        a >>= 7;
        bool sMatch = 0 != a;
        uint64_t sPos = (0x0001020304050607ull * a) >> 56;
        return std::pair<bool, size_t>(sMatch, sPos);
    }

private:
    uint64_t m_Index;
};
#endif

#if 0
class CompactCharSet7
{
public:
    CompactCharSet7() { }
    CompactCharSet7(const unsigned char* aValues) { build(aValues); }
    void build(const unsigned char* aSortedValues)
    {
        m_Index = 0;
        for (size_t i = 0; i < 7; i++)
            m_Index |= (255ull - aSortedValues[i]) << (i * 9);
    }
    std::pair<bool, size_t> find(unsigned char a) const
    {
        const uint64_t sLoMask = 0x0040201008040201ull;
        const uint64_t sHiMask = 0x4020100804020100ull;
        uint64_t sMask = m_Index + (a * sLoMask);
        bool sMatch = ((sMask ^ (sMask + sLoMask)) & sHiMask) != 0;
        uint64_t sPos = (((sMask & sHiMask) >> 2) * sLoMask) >> 60;
        return std::pair<bool, size_t>(sMatch, sPos);
    }

private:
    uint64_t m_Index;
};
#endif

class CompactCharSet63
{

};