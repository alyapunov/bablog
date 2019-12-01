#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iterator>
#include <string>
#include <stdexcept>
#include <unordered_map>

#include <IndexedBitset.hpp>

template <size_t PAGE_SIZE>
class FileReader
{
    struct Page;
    static_assert((PAGE_SIZE & (PAGE_SIZE - 1)) == 0, "Must be power of 2");

public:
    FileReader(const std::string& aFileName);
    ~FileReader();

    class iterator : std::iterator<std::bidirectional_iterator_tag, char>
    {
    public:
        iterator() = delete;
        ~iterator();
        iterator(FileReader& aReader, size_t aPos);
        iterator(const iterator& a);
        iterator(iterator&& a) noexcept;
        iterator& operator=(const iterator& a);
        iterator& operator=(iterator&& a) noexcept;

        size_t pos() const { return m_Pos; }
        char operator*() const;
        char operator[](size_t aAdvance) const;
        iterator& operator++();
        iterator operator++(int);
        bool operator==(const iterator& a) const { return m_Pos == a.m_Pos; }
        bool operator!=(const iterator& a) const { return m_Pos != a.m_Pos; }

    private:
        FileReader& m_Reader;
        size_t m_Pos;
        Page* m_Page;
    };

    iterator begin() { return iterator(*this, 0); }
    iterator end() { return iterator(*this, m_Size); }
    iterator at(size_t aPos)  { return iterator(*this, aPos); }

    struct Stats
    {
        size_t m_PageSize = PAGE_SIZE;
        size_t m_PagesCount = 0;
        size_t m_PagesMaxCount = 0;
        size_t m_PagesTotalRead = 0;
        void operator++() { ++m_PagesCount; ++m_PagesTotalRead; if (m_PagesCount > m_PagesMaxCount) ++m_PagesMaxCount; }
        void operator--() { --m_PagesCount; }
    };

    const Stats& getStats() const { return m_Stats; }

private:
    FileReader(const FileReader&) = delete;
    FileReader&operator=(const FileReader&) = delete;

    struct Page
    {
        size_t m_PageNo;
        size_t m_Size;
        size_t m_ItrCount = 0;
        char m_Data[PAGE_SIZE];
        explicit Page(size_t aPageNo = 0, size_t aSize = 0) : m_PageNo(aPageNo), m_Size(aSize) {}
    };

    Page& openPage(size_t aPageNo);
    Page& getPage(size_t aPageNo);
    void closePage(Page& aPage);
    void cleanup();
    Page* bless(Page* aPage);
    void curse(Page* aPage);

    int m_Fd = -1;
    size_t m_Size;
    IndexedBitset m_PageBitset;
    std::unordered_map<size_t, Page> m_Pages;
    Stats m_Stats;
};

// FileReader
template <size_t PAGE_SIZE>
inline FileReader<PAGE_SIZE>::FileReader(const std::string& aFileName)
{
    struct stat st;
    int rc = stat(aFileName.c_str(), &st);
    if (rc != 0)
        throw std::runtime_error("Failed to find file");
    m_Size = st.st_size;
    m_Fd = open(aFileName.c_str(), O_RDONLY, 0);
    if (m_Fd < 0)
        throw std::runtime_error("Failed to open file");
    m_PageBitset.create(m_Size);
}

template <size_t PAGE_SIZE>
inline FileReader<PAGE_SIZE>::~FileReader()
{
    if (m_Fd >= 0)
        close(m_Fd);
}

template <size_t PAGE_SIZE>
inline typename FileReader<PAGE_SIZE>::Page& FileReader<PAGE_SIZE>::openPage(size_t aPageNo)
{
    assert(m_Pages.count(aPageNo) == 0);
    size_t sSize = std::min(PAGE_SIZE, m_Size - aPageNo * PAGE_SIZE);
    assert(sSize > 0);

    if (lseek(m_Fd, aPageNo * PAGE_SIZE, SEEK_SET) != static_cast<off_t>(aPageNo * PAGE_SIZE))
        throw std::runtime_error("Failed to lseek");

    auto [sItr, sDone] = m_Pages.emplace(std::piecewise_construct, std::forward_as_tuple(aPageNo), std::forward_as_tuple(aPageNo, sSize));
    assert(sDone);
    Page& sPage = sItr->second;

    size_t sReaden = 0;
    do
    {
        ssize_t rc = read(m_Fd, sPage.m_Data + sReaden, sSize - sReaden);
        if (rc > 0)
        {
            sReaden += rc;
        }
        else if (rc == 0 || errno != EINTR)
        {
            m_Pages.erase(aPageNo);
            throw std::runtime_error("Failed to read");
        }
    } while (sReaden != sSize);

    m_PageBitset.set(aPageNo);
    ++m_Stats;
    return sPage;
}

template <size_t PAGE_SIZE>
inline typename FileReader<PAGE_SIZE>::Page& FileReader<PAGE_SIZE>::getPage(size_t aPageNo)
{
    auto sItr = m_Pages.find(aPageNo);
    if (sItr != m_Pages.end())
        return sItr->second;
    else
        return openPage(aPageNo);
}

template <size_t PAGE_SIZE>
inline void FileReader<PAGE_SIZE>::closePage(Page& aPage)
{
    assert(aPage.m_ItrCount == 0);
    --m_Stats;
    m_PageBitset.clear(aPage.m_PageNo);
    m_Pages.erase(aPage.m_PageNo);
}

template <size_t PAGE_SIZE>
inline void FileReader<PAGE_SIZE>::cleanup()
{
    while (!m_Pages.empty())
    {
        size_t sPageNo = m_PageBitset.lowest();
        Page& sPage = m_Pages[sPageNo];
        if (sPage.m_ItrCount != 0)
            return;
        closePage(sPage);
    }
}

template <size_t PAGE_SIZE>
inline typename FileReader<PAGE_SIZE>::Page* FileReader<PAGE_SIZE>::bless(Page* aPage)
{
    if (aPage != nullptr)
        ++aPage->m_ItrCount;
    return aPage;
}

template <size_t PAGE_SIZE>
inline void FileReader<PAGE_SIZE>::curse(Page* aPage)
{
    if (aPage != nullptr)
        if (--aPage->m_ItrCount == 0)
            cleanup();
}

// iterator
template<size_t PAGE_SIZE>
inline FileReader<PAGE_SIZE>::iterator::iterator(FileReader &aReader, size_t aPos)
    : m_Reader(aReader)
    , m_Pos(std::min(aPos, aReader.m_Size))
    , m_Page(aReader.bless(aPos < aReader.m_Size ? &aReader.getPage(aPos / PAGE_SIZE) : nullptr))
{
    assert(m_Pos <= m_Reader.m_Size);
    assert((m_Page == nullptr) == (m_Pos >= m_Reader.m_Size));
}

template<size_t PAGE_SIZE>
inline FileReader<PAGE_SIZE>::iterator::~iterator()
{
    assert(m_Pos <= m_Reader.m_Size);
    assert((m_Page == nullptr) == (m_Pos >= m_Reader.m_Size));
    m_Reader.curse(m_Page);
}

template<size_t PAGE_SIZE>
inline FileReader<PAGE_SIZE>::iterator::iterator(const iterator& a)
    : m_Reader(a.m_Reader)
    , m_Pos(a.m_Pos)
    , m_Page(a.m_Reader.bless(a.m_Page))
{
}

template<size_t PAGE_SIZE>
inline FileReader<PAGE_SIZE>::iterator::iterator(iterator &&a) noexcept
    : m_Reader(a.m_Reader), m_Pos(a.m_Pos), m_Page(a.m_Page)
{
    a.m_Pos = m_Reader.m_Size;
    a.m_Page = nullptr;
}

template<size_t PAGE_SIZE>
inline typename FileReader<PAGE_SIZE>::iterator& FileReader<PAGE_SIZE>::iterator::operator=(const iterator& a)
{
    assert(&m_Reader == &a.m_Reader);
    m_Reader.bless(a.m_Page);
    m_Reader.curse(m_Page);
    m_Pos = a.m_Pos;
    m_Page = a.m_Page;
    return *this;
}

template<size_t PAGE_SIZE>
inline typename FileReader<PAGE_SIZE>::iterator& FileReader<PAGE_SIZE>::iterator::operator=(iterator&& a) noexcept
{
    assert(&m_Reader == &a.m_Reader);
    std::swap(m_Pos, a.m_Pos);
    std::swap(m_Page, a.m_Page);
    return *this;
}

template<size_t PAGE_SIZE>
inline char FileReader<PAGE_SIZE>::iterator::operator*() const
{
    assert(m_Pos <= m_Reader.m_Size);
    assert((m_Page == nullptr) == (m_Pos >= m_Reader.m_Size));
    if (m_Pos >= m_Reader.m_Size)
        return 0;
    return m_Page->m_Data[m_Pos % PAGE_SIZE];
}

template<size_t PAGE_SIZE>
inline char FileReader<PAGE_SIZE>::iterator::operator[](size_t aAdvance) const
{
    assert(m_Pos <= m_Reader.m_Size);
    assert((m_Page == nullptr) == (m_Pos >= m_Reader.m_Size));
    if (m_Pos + aAdvance >= m_Reader.m_Size)
        return 0;
    else if (m_Pos / PAGE_SIZE == (m_Pos + aAdvance) / PAGE_SIZE)
        return m_Page->m_Data[(m_Pos + aAdvance) % PAGE_SIZE];
    else
        return m_Reader.getPage((m_Pos + aAdvance) / PAGE_SIZE).m_Data[(m_Pos + aAdvance) % PAGE_SIZE];
}

template<size_t PAGE_SIZE>
inline typename FileReader<PAGE_SIZE>::iterator& FileReader<PAGE_SIZE>::iterator::operator++()
{
    assert(m_Pos <= m_Reader.m_Size);
    assert((m_Page == nullptr) == (m_Pos >= m_Reader.m_Size));
    if (m_Pos == m_Reader.m_Size)
        return *this;
    ++m_Pos;
    if (m_Pos >= m_Reader.m_Size || m_Pos % PAGE_SIZE == 0)
    {
        Page* sOldPage = m_Page;
        m_Page = m_Reader.bless(m_Pos < m_Reader.m_Size ? &m_Reader.getPage(m_Pos / PAGE_SIZE) : nullptr);
        m_Reader.curse(sOldPage);
    }
    return *this;
}

template<size_t PAGE_SIZE>
inline typename FileReader<PAGE_SIZE>::iterator FileReader<PAGE_SIZE>::iterator::operator++(int)
{
    iterator sRet = *this;
    ++(*this);
    return sRet;
}
