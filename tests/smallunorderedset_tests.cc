#include <catch2/catch_test_macros.hpp>
#include <string.h>
#include "container/small_unordered_set.hpp"

TEST_CASE("Insert", "[small_unordered_set]")
{
    llec::small_unordered_set<std::string, 3> set;
    CHECK(set.size() == 0);
    CHECK(set.capacity() == 3);

    CHECK(set.insert("hello").second);
    CHECK(set.insert(" world").second);
    CHECK(!set.insert(" world").second);
    CHECK(!set.insert("hello").second);
    CHECK(set.insert(" !").second);

    CHECK(set.size() == set.capacity());
}

TEST_CASE("Range insert", "[small_unordered_set]")
{
    llec::small_unordered_set<std::string, 3> set;
    std::initializer_list<std::string> il{"hello", " world", " world", "hello", " !"};
    set.insert(il.begin(), il.end());
    CHECK(set.size() == set.capacity());
}

TEST_CASE("Initializer list insert", "[small_unordered_set]")
{
    llec::small_unordered_set<std::string, 3> set;
    set.insert({"hello", " world", " world", "hello", " !"});
    CHECK(set.size() == set.capacity());
}

TEST_CASE("Erase", "[small_unordered_set]")
{
    SECTION("Iterator")
    {
        llec::small_unordered_set<std::string, 10> set;
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
        llec::small_unordered_set<std::string, 10> set;
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
        llec::small_unordered_set<std::string, 10> set;
        set.insert({"The ", "quick ", "brown ", "fox ", "jumps ", "over ", "the ", "lazy ", "dog"});
        CHECK(set.size() == set.capacity() - 1);

        CHECK(set.erase("the "));
        CHECK(!set.erase("hello"));
        CHECK(set.erase("lazy "));
        CHECK(!set.erase("lazy "));
        CHECK(set.size() == 7);
    }
}

TEST_CASE("Clear", "[small_unordered_set]")
{
    llec::small_unordered_set<std::string, 3> set;
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

TEST_CASE("Find", "[small_unordered_set]")
{
    llec::small_unordered_set<std::string, 3> set;
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

TEST_CASE("Contains", "[small_unordered_set]")
{
    llec::small_unordered_set<std::string, 3> set;
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

TEST_CASE("Move", "[small_unordered_set]")
{
    SECTION("Construction")
    {
        llec::small_unordered_set<std::string, 3> set;
        CHECK(set.size() == 0);
        CHECK(set.capacity() == 3);

        set.insert("hello");
        set.insert(" world");
        set.insert(" world");
        set.insert("hello");
        set.insert(" !");

        decltype(set) set1 = std::move(set);
        CHECK(set1.size() == set1.capacity());
        CHECK(set1.contains(" world"));
        CHECK(set1.contains("hello"));
        CHECK(set1.contains(" !"));
        CHECK(!set1.contains("world"));
    }

    SECTION("Assignment")
    {
        llec::small_unordered_set<std::string, 3> set;
        CHECK(set.size() == 0);
        CHECK(set.capacity() == 3);

        set.insert("hello");
        set.insert(" world");
        set.insert(" world");
        set.insert("hello");
        set.insert(" !");

        decltype(set) set1;
        set1 = std::move(set);
        CHECK(set1.size() == set1.capacity());
        CHECK(set1.contains(" world"));
        CHECK(set1.contains("hello"));
        CHECK(set1.contains(" !"));
        CHECK(!set1.contains("world"));
    }
}

TEST_CASE("Copy", "[small_unordered_set]")
{
    SECTION("Construction")
    {
        llec::small_unordered_set<std::string, 3> set;
        CHECK(set.size() == 0);
        CHECK(set.capacity() == 3);

        set.insert("hello");
        set.insert(" world");
        set.insert(" world");
        set.insert("hello");
        set.insert(" !");

        decltype(set) set1 = set;
        CHECK(set1.size() == set1.capacity());
        CHECK(set1.contains(" world"));
        CHECK(set1.contains("hello"));
        CHECK(set1.contains(" !"));
        CHECK(!set1.contains("world"));

        CHECK(set.size() == set.capacity());
        CHECK(set.contains(" world"));
        CHECK(set.contains("hello"));
        CHECK(set.contains(" !"));
        CHECK(!set.contains("world"));
    }

    SECTION("Assignment")
    {
        llec::small_unordered_set<std::string, 3> set;
        CHECK(set.size() == 0);
        CHECK(set.capacity() == 3);

        set.insert("hello");
        set.insert(" world");
        set.insert(" world");
        set.insert("hello");
        set.insert(" !");

        decltype(set) set1;
        set1 = set;
        CHECK(set1.size() == set1.capacity());
        CHECK(set1.contains(" world"));
        CHECK(set1.contains("hello"));
        CHECK(set1.contains(" !"));
        CHECK(!set1.contains("world"));

        CHECK(set.size() == set.capacity());
        CHECK(set.contains(" world"));
        CHECK(set.contains("hello"));
        CHECK(set.contains(" !"));
        CHECK(!set.contains("world"));
    }
}
