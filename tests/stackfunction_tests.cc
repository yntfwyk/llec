#include <catch2/catch_test_macros.hpp>
#include <utility/stack_function.hpp>
#include <utility/typeinfo.hpp>
#include <core/fancy_print.hpp>
#include <array>
#include <string>

int multiply(int x, int y)
{
    return x * y;
}
std::size_t test(const std::string& s1, std::string& s2)
{
    s2 = s1;
    return s1.length() + s2.length();
}

TEST_CASE("Stack Function", "[stack_function]")
{
    struct Add
    {
        int operator()(int x, int y) const
        {
            return x + y;
        }
    };

    llec::stack_function<8, int(int, int)> func1(multiply);
    llec::stack_function<8, int(int, int)> func2([](int x, int y) { return x + y; });
    
    llec::stack_function<8, int(int, int)> func3;
    llec::stack_function<8, std::size_t(const std::string&, std::string&)> func4 = test;
    std::string s1 = "hello";
    std::string s2 = "world";
    CHECK(func4(s1, s2) == 10);
    CHECK(s1 == s2);

    CHECK(func1(3, 3) == 9);
    CHECK(func2(0, 9) == 9);

    func3 = func2;
    CHECK(func3(6, 3) == 9);
    
    func3 = multiply;
    CHECK(func3(3, 6) == 18);

    func3 = Add{};
    CHECK(func3(8, 1) == 9);

    func2 = multiply;
    CHECK(func2(0, 9) == 0);
}

TEST_CASE("Recursion", "[stack_function]")
{
    // from cppreference
    auto factorial = [](int n)
    {
        // store a lambda object to emulate "recursive lambda"; aware of extra overhead
        llec::stack_function<32, int(int)> fac = [&](int n) { return (n < 2) ? 1 : n * fac(n - 1); };
        // note that "auto fac = [&](int n) {...};" does not work in recursive calls
        return fac(n);
    };
    const std::array<int, 3> results = {120, 720, 5040};
    for (int i{5}, j{}; i != 8; ++i, ++j)
        CHECK(factorial(i) == results[j]);

    class Check
    {
      public:
        int f(int a, int b) const
        {
            return a + b;
        }
    };
    Check c;
    llec::stack_function<32, int(Check&, int, int)> mf = &Check::f;
    CHECK(mf(c, 1, 2) == 3);
}

TEST_CASE("reset", "[stack_function]")
{
    struct Test
    {
        Test(bool* val) : m_val{val}
        {
        }

        ~Test()
        {
            *m_val = true;
        }

        void operator()() const
        {

        }

        bool* m_val{};
    };

    bool val = false;
    llec::stack_function<8, void()> func{Test{&val}};

    func.reset();

    CHECK(val);
}

TEST_CASE("operator bool()", "[stack_function]")
{
    std::string test = "HelloWorld!";
    llec::stack_function<128, int(float, std::string)> func{[&test](float f, std::string s) { return test.length(); }};
    CHECK(static_cast<bool>(func));

    func = nullptr;

    CHECK(!static_cast<bool>(func));
    
    llec::stack_function<128, int(float, std::string)> func1;
    CHECK(!static_cast<bool>(func1));
}