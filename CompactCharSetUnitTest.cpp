#include <CompactCharSet.hpp>
#include <CompactCharSetTestUtils.hpp>

#include <assert.h>

#include <iostream>
#include <stdexcept>

void check(bool aExpession, const char* aMessage)
{
    if (!aExpession)
    {
        //assert(false);
        throw std::runtime_error(aMessage);
    }
}

void test7()
{
    const size_t N = 128 * 1024;
    for (size_t i = 0; i < N; i++)
    {
        const unsigned char* data = generate(7);
        CompactCharSet7 test(data);
        OwnBinaryCharSet7 ref(data);
        for (size_t j = 0; j < 256; j++)
        {
            auto test_res = test.find(j);
            auto ref_res = ref.find(j);
            check(test_res.first == ref_res.first, "Failed: hit");
            if (test_res.first)
                check(test_res.second == ref_res.second, "Failed: position");
        }
    }
}

int main()
{
    try
    {
        test7();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}