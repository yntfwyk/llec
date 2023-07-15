#include <catch2/catch_test_macros.hpp>
#include <container/index_preservation_array.hpp>
#include <iostream>
#include <string.h>

TEST_CASE("Insert", "[index_preservation_array]")
{
    llec::idxp_array<std::string, 10> arr;
    CHECK(arr.size() == 0);

    for (llec::s32 i = 0; i < arr.capacity(); i++)
    {
        arr.insert("hello");
    }
    CHECK(arr.size() == 10);
}

TEST_CASE("Iterators", "[index_preservation_array]")
{
    SECTION("Empty array")
    {
        const llec::idxp_array<std::string, 10> arr;

        llec::s32 count{};
        for (auto&& elem : arr)
        {
            count++;
        }
        CHECK(count == 0);
    }

    SECTION("Full array")
    {
        llec::idxp_array<std::string, 10> arr;

        for (llec::s32 i = 0; i < arr.capacity(); i++)
        {
            arr.insert("string");
        }

        llec::s32 count{};
        for (auto&& elem : arr)
        {
            count++;
            if (count == arr.size())
            {
                elem = "llec";
            }
        }
        CHECK(count == arr.size());
        CHECK(arr.data()[count - 1] == "llec");
    }
}

TEST_CASE("Data", "[index_preservation_array]")
{
    llec::idxp_array<std::string, 10> arr;

    for (llec::s32 i = 0; i < arr.capacity(); i++)
    {
        arr.insert("string");
    }

    const auto& crefArr = arr;

    for (const std::string* str = crefArr.data(); str != (crefArr.data() + crefArr.size()); str++)
    {
        CHECK(*str == "string");
    }
}

TEST_CASE("Clear", "[index_preservation_array]")
{
    llec::idxp_array<std::string, 10> arr;

    for (llec::s32 i = 0; i < arr.capacity(); i++)
    {
        arr.insert("string");
    }

    CHECK(arr.size() == arr.capacity());

    arr.clear();

    CHECK(arr.size() == 0);
}

TEST_CASE("Erase", "[index_preservation_array]")
{
    llec::idxp_array<std::string, 5> arr;

    typename decltype(arr)::key_type keys[5];
    for (llec::s32 i = 0; i < arr.capacity(); i++)
    {
        keys[i] = arr.insert("string" + std::to_string(i));
    }

    CHECK(arr.size() == arr.capacity());
    CHECK(arr.erase(keys[4]));
    CHECK(!arr.erase(keys[4]));
    CHECK(arr.erase(keys[0]));
    CHECK(arr.erase(keys[2]));
    CHECK(arr.size() == 2);
}

TEST_CASE("Subscript", "[index_preservation_array]")
{
    llec::idxp_array<std::string, 5> arr;

    typename decltype(arr)::key_type keys[5];
    for (llec::s32 i = 0; i < arr.capacity(); i++)
    {
        keys[i] = arr.insert("string" + std::to_string(i));
    }
    CHECK(arr.size() == arr.capacity());
    CHECK(arr.erase(keys[4]));
    CHECK(!arr.erase(keys[4]));
    CHECK(arr.erase(keys[0]));
    CHECK(arr.erase(keys[2]));
    CHECK(!arr.is_key_valid(keys[2]));
    CHECK(arr.size() == 2);
    CHECK(arr.is_key_valid(keys[1]));
    CHECK(arr[keys[1]] == "string1");
}

TEST_CASE("Insert", "[index_preservation_array][trivial]")
{
    llec::idxp_array<llec::s32, 5> arr;

    typename decltype(arr)::key_type keys[5];
    for (llec::s32 i = 0; i < arr.capacity(); i++)
    {
        keys[i] = arr.insert(i);
    }

    CHECK(arr.size() == arr.capacity());
    CHECK(arr.erase(keys[4]));
    CHECK(!arr.erase(keys[4]));
    CHECK(arr.erase(keys[0]));
    CHECK(arr.erase(keys[2]));
    CHECK(arr.size() == 2);
}

TEST_CASE("Iterators", "[index_preservation_array][trivial]")
{
    SECTION("Empty array")
    {
        const llec::idxp_array<int, 10> arr;

        llec::s32 count{};
        for (auto&& elem : arr)
        {
            count++;
        }
        CHECK(count == 0);
    }

    SECTION("Full array")
    {
        llec::idxp_array<int, 10> arr;

        for (llec::s32 i = 0; i < arr.capacity(); i++)
        {
            arr.insert(i);
        }

        llec::s32 count{};
        for (auto&& elem : arr)
        {
            count++;
            if (count == arr.size())
            {
                elem = 9;
            }
        }
        CHECK(count == arr.size());
        CHECK(arr.data()[count - 1] == 9);
    }
}

TEST_CASE("Data", "[index_preservation_array][trivial]")
{
    llec::idxp_array<int, 10> arr;

    for (llec::s32 i = 0; i < arr.capacity(); i++)
    {
        arr.insert(9);
    }

    const auto& crefArr = arr;

    for (const int* i = crefArr.data(); i != (crefArr.data() + crefArr.size()); i++)
    {
        CHECK(*i == 9);
    }
}

TEST_CASE("Clear", "[index_preservation_array][trivial]")
{
    llec::idxp_array<int, 10> arr;

    for (llec::s32 i = 0; i < arr.capacity(); i++)
    {
        arr.insert(i);
    }

    CHECK(arr.size() == arr.capacity());

    arr.clear();

    CHECK(arr.size() == 0);
}

TEST_CASE("Erase", "[index_preservation_array][trivial]")
{
    llec::idxp_array<int, 5> arr;

    typename decltype(arr)::key_type keys[5];
    for (llec::s32 i = 0; i < arr.capacity(); i++)
    {
        keys[i] = arr.insert(i);
    }

    CHECK(arr.size() == arr.capacity());
    CHECK(arr.erase(keys[4]));
    CHECK(!arr.erase(keys[4]));
    CHECK(arr.erase(keys[0]));
    CHECK(arr.erase(keys[2]));
    CHECK(arr.size() == 2);
}

TEST_CASE("Subscript", "[index_preservation_array][trivial]")
{
    llec::idxp_array<int, 5> arr;

    typename decltype(arr)::key_type keys[5];
    for (llec::s32 i = 0; i < arr.capacity(); i++)
    {
        keys[i] = arr.insert(i);
    }

    CHECK(arr.size() == arr.capacity());
    CHECK(arr.erase(keys[4]));
    CHECK(!arr.erase(keys[4]));
    CHECK(arr.erase(keys[0]));
    CHECK(arr.erase(keys[2]));
    CHECK(!arr.is_key_valid(keys[2]));
    CHECK(arr.size() == 2);
    CHECK(arr.is_key_valid(keys[1]));
    CHECK(arr[keys[1]] == 1);
}
