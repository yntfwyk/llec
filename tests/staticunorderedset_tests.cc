#include <catch2/catch_test_macros.hpp>
#include <container/static_unordered_set.hpp>
#include <string.h>

TEST_CASE("Insert", "[static_unordered_set]")
{
    llec::static_unordered_set<std::string, 3> set;
    CHECK(set.size() == 0);
    CHECK(set.capacity() == 3);

    CHECK(set.insert("hello").second);
    CHECK(set.insert(" world").second);
    CHECK(!set.insert(" world").second);
    CHECK(!set.insert("hello").second);
    CHECK(set.insert(" !").second);

    CHECK(set.size() == set.capacity());
}

TEST_CASE("Range insert", "[static_unordered_set]")
{
    llec::static_unordered_set<std::string, 3> set;
    std::initializer_list<std::string> il{"hello", " world", " world", "hello", " !"};
    set.insert(il.begin(), il.end());
    CHECK(set.size() == set.capacity());
}

TEST_CASE("Initializer list insert", "[static_unordered_set]")
{
    llec::static_unordered_set<std::string, 3> set;
    set.insert({"hello", " world", " world", "hello", " !"});
    CHECK(set.size() == set.capacity());
}

TEST_CASE("Erase", "[static_unordered_set]")
{
    SECTION("Iterator")
    {
        llec::static_unordered_set<std::string, 10> set;
        set.insert({"The ", "quick ", "brown ", "fox ", "jumps ", "over ", "the ", "lazy ", "dog"});
        CHECK(set.size() == set.capacity() - 1);

        CHECK(set.contains("brown "));
        set.erase(set.begin() + 2);
        CHECK(!set.contains("brown "));

        CHECK(set.contains("dog"));
        set.erase(set.end() - 1);
        CHECK(!set.contains("dog"));

        CHECK(set.size() == 7);
    }

    SECTION("Range")
    {
        llec::static_unordered_set<std::string, 10> set;
        set.insert({"The ", "quick ", "brown ", "fox ", "jumps ", "over ", "the ", "lazy ", "dog"});
        CHECK(set.size() == set.capacity() - 1);

        set.erase(set.begin() + 5, set.end());
        CHECK(set.size() == 5);

        set.erase(set.cbegin() + 1, set.cbegin() + 4);
        CHECK(set.size() == 2);
        CHECK(*(set.begin()) == "The ");
        CHECK(*(set.end() - 1) == "jumps ");
    }

    SECTION("Key")
    {
        llec::static_unordered_set<std::string, 10> set;
        set.insert({"The ", "quick ", "brown ", "fox ", "jumps ", "over ", "the ", "lazy ", "dog"});
        CHECK(set.size() == set.capacity() - 1);

        CHECK(set.erase("the "));
        CHECK(!set.erase("hello"));
        CHECK(set.erase("lazy "));
        CHECK(!set.erase("lazy "));
        CHECK(set.size() == 7);
    }
}

TEST_CASE("Clear", "[static_unordered_set]")
{
    llec::static_unordered_set<std::string, 3> set;
    CHECK(set.size() == 0);
    CHECK(set.capacity() == 3);

    CHECK(set.insert("hello").second);
    CHECK(set.insert(" world").second);
    CHECK(!set.insert(" world").second);
    CHECK(!set.insert("hello").second);
    CHECK(set.insert(" !").second);

    set.clear();
    CHECK(set.size() == 0);
}

TEST_CASE("Find", "[static_unordered_set]")
{
    llec::static_unordered_set<std::string, 3> set;
    CHECK(set.size() == 0);
    CHECK(set.capacity() == 3);

    set.insert("hello");
    set.insert(" world");
    set.insert(" world");
    set.insert("hello");
    set.insert(" !");
    auto&& ref = std::as_const(set);
    CHECK(ref.size() == ref.capacity());
    CHECK(ref.find(" world") != ref.end());
    CHECK(ref.find("hello") != ref.end());
    CHECK(ref.find(" !") != ref.end());
    CHECK(ref.find("world") == ref.end());
}

TEST_CASE("Contains", "[static_unordered_set]")
{
    llec::static_unordered_set<std::string, 3> set;
    CHECK(set.size() == 0);
    CHECK(set.capacity() == 3);

    set.insert("hello");
    set.insert(" world");
    set.insert(" world");
    set.insert("hello");
    set.insert(" !");

    CHECK(set.size() == set.capacity());
    CHECK(set.contains(" world"));
    CHECK(set.contains("hello"));
    CHECK(set.contains(" !"));
    CHECK(!set.contains("world"));
}