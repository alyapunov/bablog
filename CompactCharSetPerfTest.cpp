#include <CompactCharSet.hpp>
#include <CompactCharSetTestUtils.hpp>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <tuple>
#include <variant>

const size_t N = 16 * 1024;
using TestTypes = std::tuple<CompactCharSet7[N], LinearCharSet7[N], LinearHintCharSet7[N], OwnBinaryCharSet7[N],
    StdLowerBound7[N], MemChr7[N], StdFind7[N], StdStrFind7[N], StdStrViewFind7[N]>;

TestTypes arrs;
unsigned char fnd[N];

template <class T>
const char* name(const T*)
{
    return T::name();
}

const char* name(const CompactCharSet7*)
{
    return "Test   ";
}

static void checkpoint(const char* aText, size_t aOpCount)
{
    using namespace std::chrono;
    high_resolution_clock::time_point now = high_resolution_clock::now();
    static high_resolution_clock::time_point was;
    duration<double> time_span = duration_cast<duration<double>>(now - was);
    if (0 != aOpCount)
    {
        double Mrps = aOpCount / 1000000. / time_span.count();
        std::cout << aText << ":\t" << Mrps << " Mrps" << std::endl;
    }
    was = now;
}

template <class T>
void run(T* aCont)
{
    size_t sum[2] = {0};
    checkpoint("", 0);
    for (size_t i = 0; i < N; i++)
    {
        for (size_t j = 0; j < N; j++)
        {
            auto r = aCont[i].find(fnd[j]);
            sum[r.first] += r.second;
        }
    }
    checkpoint(name(aCont), N * N);
    std::cout << "Check: " << sum[1] << std::endl;
}

int main()
{
    std::tuple<int, double> test = {22, 1.5};
    auto ff = [](const auto& x) { std::cout << x << std::endl; };
    std::apply([ff](auto& ... a){ (..., ff(a)); }, test);

    srand(time(nullptr));
    size_t lims[] = {20, 50 ,100, 256};
    for (size_t lim : lims)
    {
        std::cout << "Limit: " << lim << std::endl;
        for (size_t i = 0; i < N; i++)
        {
            const unsigned char* data = generate(7, lim);
            std::apply([i, data](auto& ... a) { (..., a[i].build(data)); }, arrs);
            fnd[i] = rand() % lim;
        }

        std::apply([](auto& ...  a){ (..., run(a)); }, arrs);

        std::cout << std::endl;
    }

}