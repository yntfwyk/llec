#include <catch2/catch_test_macros.hpp>
#include <container/fixed_string.hpp>

TEST_CASE("FixedString length", "[fixed_string][length]")
{
    llec::fixed_string sstr{"Hello World!"};
    CHECK(sstr.length() == 12);
}

TEST_CASE("FixedString iteration", "[fixed_string][iteration]")
{
    const char* str = "Hello World!";

    SECTION("Non-const iteration")
    {
        llec::fixed_string sstr{"Hello World!"};
        llec::s32 i{};
        for (auto&& c : sstr)
        {
            CHECK(c == str[i++]);
        }
    }

    SECTION("Const iteration")
    {
        const llec::fixed_string sstr{"Hello World!"};
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
        llec::fixed_string sstr1{"Hello World!"};
        llec::fixed_string sstr2{"HelloWorld!"};

        CHECK(sstr1 != sstr2);
    }

    SECTION("Equal!")
    {
        llec::fixed_string sstr1{"Hello World!"};
        llec::fixed_string sstr2{"Hello World!"};

        CHECK(sstr1 == sstr2);
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
        const llec::fixed_string sstr{"Hello"};

        CHECK(sstr[4] == 'o');
    }
}

TEST_CASE("FixedString find", "[fixed_string][find]")
{
    llec::fixed_string sstr{"The quick brown fox jumps over the lazy dog"};

    CHECK(sstr.find(llec::fixed_string{"fox"}) == 16);
    CHECK(sstr.find(llec::fixed_string{"The"}) == 0);
    CHECK(sstr.find(llec::fixed_string{" quick"}) == 3);
    CHECK(sstr.find(llec::fixed_string{"dog"}) == 40);
    CHECK(sstr.find(llec::fixed_string{"dog "}) == -1);
}

TEST_CASE("FixedString substr", "[fixed_string][substr]")
{
    {
        llec::fixed_string sstr{"Hello World!"};
        const llec::fixed_string hello = sstr.substr(0, 6);
        const llec::fixed_string world = sstr.substr(6, 6);

        CHECK(hello == llec::fixed_string{"Hello "});
        CHECK(world == llec::fixed_string{"World!"});
    }
    {
        llec::fixed_string sstr{"Hello"};

        CHECK(sstr.substr(0, 6) == llec::fixed_string{"Hello"});
        CHECK(sstr.substr(0, 5) == llec::fixed_string{"Hello"});
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
        llec::fixed_string sstr{};
        CHECK(sstr.capacity() == 512ull);
    }

    {
        llec::fixed_string<256> sstr{};
        CHECK(sstr.capacity() == 256ull);
    }

    {
        llec::fixed_string<128> sstr{};
        CHECK(sstr.capacity() == 128ull);
    }
}
