#include <catch2/catch_test_macros.hpp>
#include "container/fixed_string.hpp"

TEST_CASE("FixedString length", "[fixed_string][length]")
{
    constexpr llec::fixed_string sstr{"Hello World!"};
    STATIC_CHECK(sstr.length() == 12);
}

TEST_CASE("FixedString iteration", "[fixed_string][iteration]")
{
    const char* str = "Hello World!";

    SECTION("Non-const iteration")
    {
        llec::fixed_string sstr{"Hello World!"};
        auto it = sstr.begin();
        it[0] = 'h';
        CHECK(it[0] == 'h');
        CHECK(*(it + 5) == ' ');
        auto endIt = sstr.end();
        CHECK(*(endIt - 1) == '!');
        *it = 'H';
        llec::s32 i{};
        for (auto&& c : sstr)
        {
            CHECK(c == str[i++]);
        }
    }

    SECTION("Const iteration")
    {
        constexpr llec::fixed_string sstr{"Hello World!"};
        auto it = sstr.begin();
        CHECK(it[0] == 'H');
        llec::s32 i{};
        for (auto&& c : sstr)
        {
            CHECK(c == str[i++]);
        }
    }
}

TEST_CASE("FixedString comparison", "[fixed_string][comparison]")
{
    SECTION("Not equal!")
    {
        constexpr llec::fixed_string sstr1{"Hello World!"};
        constexpr llec::fixed_string sstr2{"HelloWorld!"};

        STATIC_CHECK(sstr1 != sstr2);
    }

    SECTION("Equal!")
    {
        constexpr llec::fixed_string sstr1{"Hello World!"};
        constexpr llec::fixed_string sstr2{"Hello World!"};

        STATIC_CHECK(sstr1 == sstr2);
    }
}

TEST_CASE("FixedString concatenate", "[fixed_string][concatenate]")
{
    llec::fixed_string sstr1{"Hello"};
    llec::fixed_string sstr2{" World!"};

    sstr1 = sstr1 + sstr2;
    CHECK(sstr1 == llec::fixed_string{"Hello World!"});

    sstr1 = "";
    sstr2 = sstr1 + sstr2;
    CHECK(sstr2 == llec::fixed_string{" World!"});
}

TEST_CASE("FixedString overflow", "[fixed_string][overflow]")
{
    using small_string = llec::fixed_string<8>;
    small_string sstr1{"Hello"};
    small_string sstr2{" World!"};

    sstr1 = sstr1 + sstr2;
    CHECK(sstr1 == small_string{"Hello W"});
}

TEST_CASE("FixedString subscript", "[fixed_string][subscript]")
{
    SECTION("Non-const")
    {
        llec::fixed_string sstr{"Hello"};

        sstr[0] = 'h';

        CHECK(sstr == llec::fixed_string{"hello"});
    }

    SECTION("Const")
    {
        constexpr llec::fixed_string sstr{"Hello"};

        STATIC_CHECK(sstr[4] == 'o');
    }
}

TEST_CASE("FixedString find", "[fixed_string][find]")
{
    constexpr llec::fixed_string sstr{"The quick brown fox jumps over the lazy dog"};

    STATIC_CHECK(sstr.find(llec::fixed_string{"fox"}) == 16);
    STATIC_CHECK(sstr.find(llec::fixed_string{"The"}) == 0);
    STATIC_CHECK(sstr.find(llec::fixed_string{" quick"}) == 3);
    STATIC_CHECK(sstr.find(llec::fixed_string{"dog"}) == 40);
    STATIC_CHECK(sstr.find(llec::fixed_string{"dog "}) == -1);
}

TEST_CASE("FixedString substr", "[fixed_string][substr]")
{
    {
        constexpr llec::fixed_string sstr{"Hello World!"};
        constexpr llec::fixed_string hello = sstr.substr(0, 6);
        constexpr llec::fixed_string world = sstr.substr(6, 6);

        STATIC_CHECK(hello == llec::fixed_string{"Hello "});
        STATIC_CHECK(world == llec::fixed_string{"World!"});
    }
    {
        constexpr llec::fixed_string sstr{"Hello"};

        STATIC_CHECK(sstr.substr(0, 6) == llec::fixed_string{"Hello"});
        STATIC_CHECK(sstr.substr(0, 5) == llec::fixed_string{"Hello"});
    }
}

TEST_CASE("FixedString clear", "[fixed_string][clear]")
{
    llec::fixed_string sstr{"Hello World!"};
    sstr.clear();
    CHECK(sstr.length() == 0);
}

TEST_CASE("FixedString capacity", "[fixed_string][capacity]")
{
    {
        constexpr llec::fixed_string sstr{};
        STATIC_CHECK(sstr.capacity() == 512ull);
    }

    {
        constexpr llec::fixed_string<256> sstr{};
        STATIC_CHECK(sstr.capacity() == 256ull);
    }

    {
        llec::fixed_string<128> sstr{};
        STATIC_CHECK(sstr.capacity() == 128ull);
    }
}

TEST_CASE("FixedString append", "[fixed_string][append]")
{
    {
        llec::fixed_string sstr{"Hello"};
        llec::fixed_string sstr2{" World!"};
        CHECK(sstr.append(sstr2) == llec::fixed_string("Hello World!"));
    }

    {
        llec::fixed_string<8> sstr{"Hello"};
        llec::fixed_string<8> sstr2{" World!"};
        CHECK(sstr.append(sstr2) == llec::fixed_string<8>("Hello Wo"));
    }

    {
        llec::fixed_string<8> sstr{"Hello"};
        llec::fixed_string<8> sstr2{};
        CHECK(sstr.append(sstr2) == llec::fixed_string<8>("Hello"));
    }

    {
        llec::fixed_string<6> sstr{"Hello"};
        llec::fixed_string<6> sstr2{" World!"};
        CHECK(sstr.append(sstr2) == llec::fixed_string<6>("Hello"));
    }
}
