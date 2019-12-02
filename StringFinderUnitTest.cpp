#include <StringFinder.hpp>

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

#define CHECK(expr) check(expr, #expr);

template <class T>
const char* var() { return "(? ? ?)"; }

template<>
const char* var<unsigned char>() { return " (uchar)"; }
template<>
const char* var<int>() { return " (int)"; }
template<>
const char* var<size_t>() { return " (size_t)"; }

std::vector<size_t> a; // StringFinder
std::vector<size_t> b; // Reference;
int rc = EXIT_SUCCESS;

template <class SIZE_TYPE>
void calc_a(std::string_view aNeedle, std::string_view aHayStack)
{
    a.clear();
    StringFinder<SIZE_TYPE> cf(aNeedle);
    for (size_t i = 0; i < aHayStack.size(); i++)
        if (cf.feed(aHayStack[i]))
            a.push_back(i + 1 - aNeedle.size());
}

void calc_b(std::string_view aNeedle, std::string_view aHayStack)
{
    b.clear();
    size_t pos = SIZE_MAX;
    while (true)
    {
        pos = aHayStack.find(aNeedle, pos + 1);
        if (pos == aHayStack.npos)
            break;
        b.push_back(pos);
    }
}

template <class SIZE_TYPE>
void compare(std::string_view aNeedle, std::string_view aHayStack)
{
    calc_a<SIZE_TYPE>(aNeedle, aHayStack);
    calc_b(aNeedle, aHayStack);
    if (a != b)
    {
        std::cout << "Wrong search of \"" <<  aNeedle << "\" in \"" << aHayStack << "\" " << var<SIZE_TYPE>() << "\n";
        std::cout << "Found:    ";
        for (size_t i = 0; i < a.size(); i++)
            std::cout << (i ? ", " : "") << a[i];
        std::cout << "\n";
        std::cout << "Expected: ";
        for (size_t i = 0; i < b.size(); i++)
            std::cout << (i ? ", " : "") << b[i];
        std::cout << "\n";
        rc = false;
    }
}

template <class SIZE_TYPE>
void simple_test()
{
    compare<SIZE_TYPE>("aba", "ababab");
    compare<SIZE_TYPE>("abcabd", "abcabcabdabd");
    compare<SIZE_TYPE>("aac", "aaaaaacaaaabaacaccaac");
    compare<SIZE_TYPE>("ac", "aaaaaacaaaabaacaccaac");
    compare<SIZE_TYPE>("a", "aaaaaacaaaabaacaccaac");
    compare<SIZE_TYPE>("aaa", "aaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaa");
    compare<SIZE_TYPE>("aaa", "aabaabaabaabaabaabaabaab");
    compare<SIZE_TYPE>("ab", "cdcdcdcd");
    compare<SIZE_TYPE>("abc", "ab");
    {
        constexpr size_t S = std::min(size_t(std::numeric_limits<SIZE_TYPE>::max()), size_t(64 * 1024));
        std::string s1(S, 'a');
        std::string s2(S, 'b');
        compare<SIZE_TYPE>(s1, s1);
        compare<SIZE_TYPE>(s1, s2);
    }
}

template <class SIZE_TYPE>
void massive_test()
{
    const size_t ALPH = 8;
    const size_t ROUNDS = 1024;
    std::string needle;
    std::string haystack;
    for (size_t i = 0; i < ROUNDS; i++)
    {
        for (size_t al = 2; al <= ALPH; al++)
        {
            size_t nl = 1 + rand() % 4;
            size_t nh = rand() % 128;
            auto gen = [](std::string& s, size_t l, size_t al)
            {
                s.clear();
                for (size_t i = 0; i < l; i++)
                {
                    s += static_cast<char>(0 - rand() % al);
                }
            };
            gen(needle, nl, al);
            gen(haystack, nh, al);
            compare<SIZE_TYPE>(needle, haystack);
        }
    }
}

template <class SIZE_TYPE>
void test()
{
    simple_test<SIZE_TYPE>();
    massive_test<SIZE_TYPE>();
}

int main()
{

    try
    {
        test<size_t>();
        test<unsigned char>();
        test<short>();
        test<unsigned>();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        rc = EXIT_FAILURE;
    }
    if (rc == EXIT_SUCCESS)
        std::cout << "Well done" << std::endl;
    return rc;
}
