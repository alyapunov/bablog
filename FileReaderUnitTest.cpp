#include <FileReader.hpp>

#include <cstdio>
#include <iostream>
#include <fstream>

const char* filename = "./test.dat";

void check(bool aExpession, const char* aMessage)
{
    if (!aExpession)
    {
        //assert(false);
        throw std::runtime_error(aMessage);
    }
}

#define CHECK(expr) check(expr, #expr);

struct FileRemover
{
    ~FileRemover()
    {
        if (remove(filename) != 0)
            std::cerr << "Failed to remove file!" << std::endl;
    }
};

template <size_t FILE_SIZE, size_t PAGE_SIZE>
void test1(const char* aData)
{
    FileReader<PAGE_SIZE> fr(filename);
    {
        auto sItr = fr.begin();
        if (FILE_SIZE == 0)
        {
            CHECK(sItr == fr.end());
        }
        else
        {
            CHECK(sItr != fr.end());
            CHECK(aData[0] == *sItr);
            CHECK(fr.getStats().m_PagesCount == 1);
        }
    }
    CHECK(fr.getStats().m_PagesCount == 0);
    CHECK(fr.getStats().m_PagesMaxCount <= 2);
    CHECK(fr.getStats().m_PagesTotalRead == (FILE_SIZE ? 1 : 0));
}

template <size_t FILE_SIZE, size_t PAGE_SIZE>
void test2(const char* aData)
{
    FileReader<PAGE_SIZE> fr(filename);
    auto sItr = fr.begin();
    for (size_t i = 0; i < FILE_SIZE; ++i, ++sItr)
    {
        auto sItr2(std::move(sItr));
        sItr = std::move(sItr2);
        CHECK(sItr != fr.end());
        CHECK(aData[i] == *sItr);
        CHECK(fr.getStats().m_PagesCount == 1);
    }
    CHECK(sItr == fr.end());
    CHECK(0 == *sItr);
    CHECK(fr.getStats().m_PagesCount == 0);
    CHECK(fr.getStats().m_PagesMaxCount <= 2);
    CHECK(fr.getStats().m_PagesTotalRead == (FILE_SIZE + PAGE_SIZE - 1) / PAGE_SIZE);
}

template <size_t FILE_SIZE, size_t PAGE_SIZE>
void test3(const char* aData)
{
    FileReader<PAGE_SIZE> fr(filename);
    auto sItr = fr.begin();
    for (size_t i = 0; i < FILE_SIZE; ++i, ++sItr)
    {
        CHECK(sItr != fr.end());
        CHECK(aData[i] == *sItr);
        CHECK(fr.getStats().m_PagesCount == 1);
    }
    CHECK(sItr == fr.end());
    CHECK(0 == *sItr);
    ++sItr;
    CHECK(0 == *sItr);
    CHECK(fr.getStats().m_PagesCount == 0);
    CHECK(fr.getStats().m_PagesMaxCount <= 2);
    CHECK(fr.getStats().m_PagesTotalRead == (FILE_SIZE + PAGE_SIZE - 1) / PAGE_SIZE);
}

template <size_t FILE_SIZE, size_t PAGE_SIZE>
void test4(const char* aData)
{
    FileReader<PAGE_SIZE> fr(filename);
    auto sItr = fr.begin();
    auto sItr2 = sItr;
    for (size_t i = 0; i < FILE_SIZE; ++i, ++sItr)
    {
        CHECK(sItr != fr.end());
        CHECK(aData[i] == *sItr);
    }
    CHECK(sItr == fr.end());
    CHECK(0 == *sItr);
    CHECK(fr.getStats().m_PagesCount == (FILE_SIZE + PAGE_SIZE - 1) / PAGE_SIZE);
    CHECK(fr.getStats().m_PagesMaxCount == (FILE_SIZE + PAGE_SIZE - 1) / PAGE_SIZE);
    CHECK(fr.getStats().m_PagesTotalRead == (FILE_SIZE + PAGE_SIZE - 1) / PAGE_SIZE);
}

template <size_t FILE_SIZE, size_t PAGE_SIZE>
void test5(const char* aData)
{
    FileReader<PAGE_SIZE> fr(filename);
    auto sItr = fr.begin();
    auto sItr2 = fr.begin();
    for (size_t i = 0; i < FILE_SIZE; ++i, ++sItr)
    {
        CHECK(sItr != fr.end());
        CHECK(aData[i] == *sItr);
        sItr2 = sItr;
        for (size_t j = i; j < FILE_SIZE; ++j, ++sItr2)
        {
            CHECK(sItr2 != fr.end());
            CHECK(aData[j] == *sItr2);
        }
        CHECK(sItr2 == fr.end());
    }
    CHECK(sItr == fr.end());
    CHECK(0 == *sItr);
    ++sItr;
    CHECK(0 == *sItr);
    CHECK(fr.getStats().m_PagesCount == 0);
    CHECK(fr.getStats().m_PagesMaxCount == (FILE_SIZE + PAGE_SIZE - 1) / PAGE_SIZE);
    CHECK(fr.getStats().m_PagesTotalRead == (FILE_SIZE + PAGE_SIZE - 1) / PAGE_SIZE);
}

template <size_t FILE_SIZE, size_t PAGE_SIZE>
void test6(const char* aData)
{
    FileReader<PAGE_SIZE> fr(filename);
    auto sItr = fr.begin();
    for (size_t i = 0; i < FILE_SIZE; ++i, ++sItr)
    {
        CHECK(sItr != fr.end());
        CHECK(aData[i] == *sItr);
        for (size_t j = i; j < FILE_SIZE; ++j)
        {
            CHECK(aData[j] == sItr[j - i]);
        }
    }
    CHECK(sItr == fr.end());
    CHECK(0 == *sItr);
    ++sItr;
    CHECK(0 == *sItr);
    CHECK(fr.getStats().m_PagesCount == 0);
    CHECK(fr.getStats().m_PagesMaxCount == (FILE_SIZE + PAGE_SIZE - 1) / PAGE_SIZE);
    CHECK(fr.getStats().m_PagesTotalRead == (FILE_SIZE + PAGE_SIZE - 1) / PAGE_SIZE);
}

template <size_t FILE_SIZE, size_t PAGE_SIZE>
void test7(const char* aData)
{
    FileReader<PAGE_SIZE> fr(filename);
    auto sItr = fr.begin();
    for (size_t i = 0; i < FILE_SIZE; ++i, ++sItr)
    {
        CHECK(sItr != fr.end());
        CHECK(aData[i] == sItr[0]);
        if (i + 1 < FILE_SIZE)
            CHECK(aData[i + 1] == sItr[1]);
    }
    CHECK(sItr == fr.end());
    CHECK(0 == *sItr);
    ++sItr;
    CHECK(0 == *sItr);
    CHECK(fr.getStats().m_PagesCount == 0);
    CHECK(fr.getStats().m_PagesMaxCount <= 2);
    CHECK(fr.getStats().m_PagesTotalRead == (FILE_SIZE + PAGE_SIZE - 1) / PAGE_SIZE);
}

template <size_t FILE_SIZE, size_t PAGE_SIZE>
void test()
{
    char sData[FILE_SIZE ? FILE_SIZE : 1];
    for (size_t i = 0; i < FILE_SIZE; i++)
        sData[i] = rand();
    std::ofstream f(filename, std::fstream::out | std::fstream::trunc | std::fstream::binary);
    f.write(sData, FILE_SIZE);
    f.close();
    FileRemover fr;

    test1<FILE_SIZE, PAGE_SIZE>(sData);
    test2<FILE_SIZE, PAGE_SIZE>(sData);
    test3<FILE_SIZE, PAGE_SIZE>(sData);
    test4<FILE_SIZE, PAGE_SIZE>(sData);
    test5<FILE_SIZE, PAGE_SIZE>(sData);
    test6<FILE_SIZE, PAGE_SIZE>(sData);
    test7<FILE_SIZE, PAGE_SIZE>(sData);
}

int main()
{
    int rc = EXIT_SUCCESS;

    try
    {
        test<0, 64>();
        test<1, 64>();
        test<63, 64>();
        test<64, 64>();
        test<65, 64>();
        test<127, 64>();
        test<128, 64>();
        test<129, 64>();
        test<999, 8>();
        test<1000, 8>();
        test<1001, 8>();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        rc = EXIT_FAILURE;
    }
    return rc;
}