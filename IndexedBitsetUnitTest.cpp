#include <IndexedBitset.hpp>

#include <iostream>
#include <set>

void check(bool aExpession, const char* aMessage)
{
    if (!aExpession)
    {
        //assert(false);
        throw std::runtime_error(aMessage);
    }
}

void test(size_t aBitCount)
{
    size_t sRuns = 1000 + aBitCount * 10;
    IndexedBitset b(aBitCount);
    std::set<size_t> c;
    for (size_t i = 0; i < sRuns; i++)
    {
        bool sIns = rand() & 1;
        size_t sPos = rand() % aBitCount;
        if (sIns)
        {
            b.set(sPos);
            c.insert(sPos);
        }
        else
        {
            b.clear(sPos);
            c.erase(sPos);
        }
        check(b.empty() == c.empty(), "empty check failed");
        if (!b.empty())
            check(b.lowest() == *c.begin(), "lowest check failed");
    }
}

int main()
{
    try
    {
        IndexedBitset b;
        check(b.empty(), "why no empty");
        b.create(10);
        check(b.empty(), "why no empty");
        b.create(0);
        check(b.empty(), "why no empty");
        test(1);
        test(2);
        test(64);
        test(65);
        test(1000);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

}