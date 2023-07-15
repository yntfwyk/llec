#include <catch2/catch_test_macros.hpp>

#include <container/static_string.hpp>

TEST_CASE("StaticString length", "[static_string][length]")
{
    llec::static_string sstr{"Hello World!"};
    CHECK(sstr.length() == 12);
}

TEST_CASE("StaticString iteration", "[static_string][iteration]")
{
    const char* str = "Hello World!";

    SECTION("Non-const iteration")
    {
        llec::static_string sstr{"Hello World!"};
        llec::s32 i{};
        for (auto&& c : sstr)
        {
            CHECK(c == str[i++]);
        }
    }

    SECTION("Const iteration")
    {
        const llec::static_string sstr{"Hello World!"};
        llec::s32 i{};
        for (auto&& c : sstr)
        {
            CHECK(c == str[i++]);
        }
    }
}

TEST_CASE("StaticString comparison", "[static_string][comparison]")
{
    SECTION("Not equal!")
    {
        llec::static_string sstr1{"Hello World!"};
        llec::static_string sstr2{"HelloWorld!"};

        CHECK(sstr1 != sstr2);
    }

    SECTION("Equal!")
    {
        llec::static_string sstr1{"Hello World!"};
        llec::static_string sstr2{"Hello World!"};

        CHECK(sstr1 == sstr2);
    }
}

TEST_CASE("StaticString concatenate", "[static_string][concatenate]")
{
    llec::static_string sstr1{"Hello"};
    llec::static_string sstr2{" World!"};

    sstr1 = sstr1 + sstr2;
    CHECK(sstr1 == llec::static_string{"Hello World!"});

    sstr1 = "";
    sstr2 = sstr1 + sstr2;
    CHECK(sstr2 == llec::static_string{" World!"});
}

TEST_CASE("StaticString overflow", "[static_string][overflow]")
{
    using small_string = llec::static_string<8>;
    small_string sstr1{"Hello"};
    small_string sstr2{" World!"};

    sstr1 = sstr1 + sstr2;
    CHECK(sstr1 == small_string{"Hello W"});
}

TEST_CASE("StaticString subscript", "[static_string][subscript]")
{
    SECTION("Non-const")
    {
        llec::static_string sstr{"Hello"};

        sstr[0] = 'h';

        CHECK(sstr == llec::static_string{"hello"});
    }

    SECTION("Const")
    {
        const llec::static_string sstr{"Hello"};

        CHECK(sstr[4] == 'o');
    }
}

TEST_CASE("StaticString find", "[static_string][find]")
{
    llec::static_string sstr{"The quick brown fox jumps over the lazy dog"};

    CHECK(sstr.find(llec::static_string{"fox"}) == 16);
    CHECK(sstr.find(llec::static_string{"The"}) == 0);
    CHECK(sstr.find(llec::static_string{" quick"}) == 3);
    CHECK(sstr.find(llec::static_string{"dog"}) == 40);
    CHECK(sstr.find(llec::static_string{"dog "}) == -1);
}

TEST_CASE("StaticString substr", "[static_string][substr]")
{
    {
        llec::static_string sstr{"Hello World!"};
        const llec::static_string hello = sstr.substr(0, 6);
        const llec::static_string world = sstr.substr(6, 6);

        CHECK(hello == llec::static_string{"Hello "});
        CHECK(world == llec::static_string{"World!"});
    }
    {
        llec::static_string sstr{"Hello"};

        CHECK(sstr.substr(0, 6) == llec::static_string{"Hello"});
        CHECK(sstr.substr(0, 5) == llec::static_string{"Hello"});
    }
}

TEST_CASE("StaticString clear", "[static_string][clear]")
{
    llec::static_string sstr{"Hello World!"};
    sstr.clear();
    CHECK(sstr.length() == 0);
}

TEST_CASE("StaticString capacity", "[static_string][capacity]")
{
    {
        llec::static_string sstr{};
        CHECK(sstr.capacity() == 512ull);
    }

    {
        llec::static_string<256> sstr{};
        CHECK(sstr.capacity() == 256ull);
    }

    {
        llec::static_string<128> sstr{};
        CHECK(sstr.capacity() == 128ull);
    }
}
