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