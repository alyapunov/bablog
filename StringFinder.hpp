#pragma once

#include <array>
#include <climits>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <vector>

// std::numeric_limits<SIZE_TYPE>::max() limits search string length.
template <class SIZE_TYPE = size_t, class CHAR = char>
class StringFinder
{
public:
    static_assert(std::is_integral_v<CHAR>, "Type expected to be integral");
    StringFinder() {}
    StringFinder(std::basic_string_view<CHAR> aNeedle) { create(aNeedle); }
    void create(std::basic_string_view<CHAR> aNeedle);
    bool feed(CHAR c);
    void restart() { m_CurPos = 0; }

private:
    using arr_t = std::array<SIZE_TYPE, 1ull << (sizeof(CHAR) * CHAR_BIT)>;

    static size_t cast(CHAR c) { return static_cast<size_t>(static_cast< std::make_unsigned_t<CHAR> >(c)); }

    std::vector<arr_t> m_Index;
    SIZE_TYPE m_CurPos;
    SIZE_TYPE m_FinPos;
    SIZE_TYPE m_RepPos;
};

template <typename SIZE_TYPE, typename CHAR>
inline void StringFinder<SIZE_TYPE, CHAR>::create(std::basic_string_view<CHAR> aNeedle)
{
    if (aNeedle.size() == 0)
        throw std::runtime_error("Cannot search an empty string");
    if (aNeedle.size() > static_cast<size_t>(std::numeric_limits<SIZE_TYPE>::max()))
        throw std::runtime_error("Search string is too big");

    m_Index.resize(aNeedle.size());
    m_Index[0] = {};
    m_CurPos = m_FinPos = m_RepPos = 0;
    for (size_t i = 0; i < aNeedle.size(); i++)
    {
        size_t u = cast(aNeedle[i]);
        ++m_FinPos;
        m_Index[i] = m_Index[m_RepPos];
        m_RepPos = m_Index[i][u];
        m_Index[i][u] = m_FinPos;
    }
}

template <typename SIZE_TYPE, typename CHAR>
inline bool StringFinder<SIZE_TYPE, CHAR>::feed(CHAR c)
{
    m_CurPos = m_Index[m_CurPos][cast(c)];
    if (m_CurPos == m_FinPos)
    {
        m_CurPos = m_RepPos;
        return true;
    }
    return false;
}
