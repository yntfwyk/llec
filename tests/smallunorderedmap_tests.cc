#include <catch2/catch_test_macros.hpp>
#include <container/small_unordered_map.hpp>
#include <vector>

TEST_CASE("Insert", "[small_unordered_map]")
{
    SECTION("Regular inserts")
    {
        llec::small_unordered_map<std::string, int, 4> map;
        CHECK(map.size() == 0);
        CHECK(map.capacity() == 4);
        const std::string s = "hello";
        int x = 1;
        decltype(map)::value_type p = {s, x};
        CHECK(map.insert(p).second);
        CHECK(map.insert({" world", 2}).second);
        CHECK(!map.insert({" world", 3}).second);
        CHECK(!map.insert({"hello", 4}).second);
        CHECK(map.insert({" !", 5}).second);
        CHECK(!map.insert({" !", 6}).second);
        CHECK(map.size() == 3);
        CHECK(map["hello"] == 1);
        map["hello"] = 9;
        CHECK(map[" world"] == 2);
        CHECK(map[" !"] == 5);
        CHECK(map["hello"] == 9);
    }

    SECTION("Range inserts")
    {
        llec::small_unordered_map<std::string, int, 10> map;
        map.insert({{"hello", 1}, {"world", 2}, {"!", 3}});
        std::vector <llec::pair<std::string, int>> v{{"world", 4}, {"inserts", 4}, {"!", 6}};
        CHECK(map.size() == 3);
        map.insert(v.begin(), v.end());
        CHECK(map.capacity() == 10);
        CHECK(map.size() == 4);

        llec::s32 i = 1;
        for (auto&& elem : map)
        {
            CHECK(*(elem.second) == i++);
        }

        map.clear();
        CHECK(map.size() == 0);
    }
}

TEST_CASE("Erase", "[small_unordered_map]")
{
    llec::small_unordered_map<std::string, int, 10> map;
    map["small"] = 1;
    map["unordered"] = 2;
    map["map"] = 3;
    map["erasure"] = 4;
    map["test"] = 5;
    map["case"] = 6;

    CHECK(map.size() == 6);

    CHECK(map.erase("case"));
    CHECK(!map.erase("hello"));
    CHECK(map.size() == 5);
    auto it = map.erase(map.cbegin() + 1, map.cbegin() + 4);
    CHECK(it.get_value() == 5);
    CHECK(it.get_key() == "test");
    map.at("test") = 7;
    const auto& cmap = std::as_const(map);
    CHECK(cmap.find("test").get_value() == map["test"]);
    CHECK(cmap.find("test1") == cmap.end());
    CHECK(!cmap.contains("test1"));
    CHECK(cmap.contains("small"));
    CHECK(!cmap.contains("erasure"));
    CHECK(cmap.at("test") == 7);

    map.erase(map.begin(), map.end());
    CHECK(cmap.size() == 0);
    CHECK(cmap.size() == map.size());
}

TEST_CASE("Copy", "[small_unordered_map]")
{
    SECTION("Construction")
    {
        llec::small_unordered_map<std::string, int, 10> map;
        map["small"] = 1;
        map["unordered"] = 2;
        map["map"] = 3;
        map["erasure"] = 4;
        map["test"] = 5;
        map["case"] = 6;

        llec::small_unordered_map<std::string, int, 10> copy = map;

        CHECK(copy.size() == map.size());

        for (auto&& elem : copy)
        {
            CHECK(*(elem.second) == map[*(elem.first)]);
        }
    }

    SECTION("Assignment")
    {
        llec::small_unordered_map<std::string, int, 10> map;
        map["small"] = 1;
        map["unordered"] = 2;
        map["map"] = 3;
        map["erasure"] = 4;
        map["test"] = 5;
        map["case"] = 6;

        llec::small_unordered_map<std::string, int, 10> copy;

        copy = map;

        CHECK(copy.size() == map.size());

        for (auto&& elem : copy)
        {
            CHECK(*(elem.second) == map[*(elem.first)]);
        }
    }
}

TEST_CASE("Move", "[small_unordered_map]")
{
    SECTION("Construction")
    {
        llec::small_unordered_map<std::string, int, 10> map;
        map["small"] = 1;
        map["unordered"] = 2;
        map["map"] = 3;
        map["erasure"] = 4;
        map["test"] = 5;
        map["case"] = 6;

        llec::small_unordered_map<std::string, int, 10> relocated = std::move(map);

        CHECK(relocated.size() == 6);
        llec::s32 i = 0;
        for (auto&& elem : relocated)
        {
            CHECK(*(elem.second) == ++i);
        }
    }

    SECTION("Assignment")
    {
        llec::small_unordered_map<std::string, int, 10> map;
        map["small"] = 1;
        map["unordered"] = 2;
        map["map"] = 3;
        map["erasure"] = 4;
        map["test"] = 5;
        map["case"] = 6;

        llec::small_unordered_map<std::string, int, 10> relocated;

        relocated = std::move(map);

        CHECK(relocated.size() == 6);

        llec::s32 i = 0;
        for (auto&& elem : relocated)
        {
            CHECK(*(elem.second) == ++i);
        }
    }
}
