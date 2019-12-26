#pragma once

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <string>
#include <string_view>

class LinearCharSet7
{
public:
    LinearCharSet7() { }
    LinearCharSet7(const unsigned char* aValues) { build(aValues); }
    void build(const unsigned char* aSortedValues)
    {
        for (size_t i = 0; i < 7; i++)
            m_Values[i] = aSortedValues[i];
    }
    std::pair<bool, size_t> find(unsigned char a) const
    {
        for (size_t i = 0; i < 7; i++)
        {
            if (m_Values[i] == a)
                return std::pair<bool, size_t>(true, i);
//            else if (m_Values[i] > a)
//                return std::pair<bool, size_t>(false, i);
        }
        return std::pair<bool, size_t>(false, 7);
    }
    static const char* name() { return "Linear   "; }

private:
    unsigned char m_Values[7];
};

class LinearHintCharSet7
{
public:
    LinearHintCharSet7() { }
    LinearHintCharSet7(const unsigned char* aValues) { build(aValues); }
    void build(const unsigned char* aSortedValues)
    {
        for (size_t i = 0; i < 7; i++)
            m_Values[i] = aSortedValues[i];
    }
    std::pair<bool, size_t> find(unsigned char a) const
    {
        for (size_t i = 0; i < 7; i++)
        {
            if (m_Values[i] == a)
                return std::pair<bool, size_t>(true, i);
            else if (m_Values[i] > a)
                return std::pair<bool, size_t>(false, 7);
        }
        return std::pair<bool, size_t>(false, 7);
    }
    static const char* name() { return "Linear+hint"; }

private:
    unsigned char m_Values[7];
};

class OwnBinaryCharSet7
{
public:
    OwnBinaryCharSet7() { }
    OwnBinaryCharSet7(const unsigned char* aValues) { build(aValues); }
    void build(const unsigned char* aSortedValues)
    {
        for (size_t i = 0; i < 7; i++)
            m_Values[i] = aSortedValues[i];
    }
    std::pair<bool, size_t> find(unsigned char a) const
    {
        size_t i = 3;
        if (m_Values[i] == a)
            return std::pair<bool, size_t>(true, i);
        else if (m_Values[i] > a)
            i -= 2;
        else
            i += 2;

        if (m_Values[i] == a)
            return std::pair<bool, size_t>(true, i);
        else if (m_Values[i] > a)
            i -= 1;
        else
            i += 1;

        if (m_Values[i] == a)
            return std::pair<bool, size_t>(true, i);
//        else if (m_Values[i] > a)
//            return std::pair<bool, size_t>(false, i);
//        else
//            return std::pair<bool, size_t>(false, i + 1);
        else
            return std::pair<bool, size_t>(false, 0);
    }
    static const char* name() { return "Binary (own)"; }

private:
    unsigned char m_Values[7];
};

class StdLowerBound7
{
public:
    StdLowerBound7() { }
    StdLowerBound7(const unsigned char* aValues) { build(aValues); }
    void build(const unsigned char* aSortedValues)
    {
        for (size_t i = 0; i < 7; i++)
            m_Values[i] = aSortedValues[i];
    }
    std::pair<bool, size_t> find(unsigned char a) const
    {
        auto p = std::lower_bound(m_Values, m_Values + 7, a);
        return std::pair<bool, size_t>(p < m_Values + 7 && *p == a, p - m_Values);
    }
    static const char* name() { return "Binary (std)"; }

private:
    unsigned char m_Values[7];
};

class MemChr7
{
public:
    MemChr7() { }
    MemChr7(const unsigned char* aValues) { build(aValues); }
    void build(const unsigned char* aSortedValues)
    {
        for (size_t i = 0; i < 7; i++)
            m_Values[i] = aSortedValues[i];
    }
    std::pair<bool, size_t> find(unsigned char a) const
    {
        const unsigned char* r = (const unsigned char*)memchr(m_Values, a, 7);
        bool found = r != nullptr;
        if (!found)
            r = m_Values;
        return std::pair<bool, size_t>(found, r - m_Values);
    }
    static const char* name() { return "memchr   "; }

private:
    unsigned char m_Values[7];
};

class StdFind7
{
public:
    StdFind7() { }
    StdFind7(const unsigned char* aValues) { build(aValues); }
    void build(const unsigned char* aSortedValues)
    {
        for (size_t i = 0; i < 7; i++)
            m_Values[i] = aSortedValues[i];
    }
    std::pair<bool, size_t> find(unsigned char a) const
    {
        const unsigned char* b = m_Values;
        const unsigned char* e = m_Values + 7;
        const unsigned char* r = std::find(b, e, a);
        return std::pair<bool, size_t>(r != e, r - m_Values);
    }
    static const char* name() { return "std::find"; }

private:
    unsigned char m_Values[7];
};

class StdStrFind7
{
public:
    StdStrFind7() { }
    StdStrFind7(const unsigned char* aValues) { build(aValues); }
    void build(const unsigned char* aSortedValues)
    {
        m_Str = std::string((const char*)aSortedValues, 7);
    }
    std::pair<bool, size_t> find(unsigned char a) const
    {
        size_t r = m_Str.find(a);
        return std::pair<bool, size_t>(r != m_Str.npos, r);
    }
    static const char* name() { return "string::find"; }

private:
    std::string m_Str;
};

class StdStrViewFind7
{
public:
    StdStrViewFind7() { }
    StdStrViewFind7(const unsigned char* aValues) { build(aValues); }
    void build(const unsigned char* aSortedValues)
    {
        for (size_t i = 0; i < 7; i++)
            m_Values[i] = aSortedValues[i];
    }
    std::pair<bool, size_t> find(unsigned char a) const
    {
        std::string_view str((const char*)m_Values, 7);
        size_t r = str.find(a);
        return std::pair<bool, size_t>(r != str.npos, r);
    }
    static const char* name() { return "string_v::find"; }

private:
    unsigned char m_Values[7];
};

inline const unsigned char* generate(size_t n, size_t limit = 256)
{
    thread_local unsigned char res[256];
    std::bitset<256> got_bitmask;
    size_t res_size = 0;
    while (res_size < n)
    {
        size_t rnd = rand() % limit;
        if (!got_bitmask.test(rnd))
        {
            got_bitmask.set(rnd);
            res[res_size++] = rnd;
        }
    }
    std::sort(res, res + n);
    return res;
}