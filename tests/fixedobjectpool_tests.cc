#include <catch2/catch_test_macros.hpp>
#include <container/fixed_object_pool.hpp>
#include <iostream>
#include <string.h>

TEST_CASE("Insert", "[fixed_object_pool]")
{
    llec::fop<std::string, 10> arr;
    CHECK(arr.size() == 0);

    for (llec::s32 i = 0; i < arr.capacity(); i++)
    {
        arr.insert("hello");
    }
    CHECK(arr.size() == 10);
}

TEST_CASE("Iterators", "[fixed_object_pool]")
{
    SECTION("Empty array")
    {
        const llec::fop<std::string, 10> arr;

        llec::s32 count{};
        for ([[maybe_unused]] auto&& elem : arr)
        {
            count++;
        }
        CHECK(count == 0);
    }

    SECTION("Full array")
    {
        llec::fop<std::string, 10> arr;

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

TEST_CASE("Data", "[fixed_object_pool]")
{
    llec::fop<std::string, 10> arr;

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

TEST_CASE("Clear", "[fixed_object_pool]")
{
    llec::fop<std::string, 10> arr;

    for (llec::s32 i = 0; i < arr.capacity(); i++)
    {
        arr.insert("string");
    }

    CHECK(arr.size() == arr.capacity());

    arr.clear();

    CHECK(arr.size() == 0);
}

TEST_CASE("Erase", "[fixed_object_pool]")
{
    llec::fop<std::string, 5> arr;

    typename decltype(arr)::handle keys[5];
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

TEST_CASE("Copy operation", "[fixed_object_pool]")
{
    SECTION("Construction")
    {
        llec::fop<std::string, 5> arr;

        typename decltype(arr)::handle keys[5];
        for (llec::s32 i = 0; i < arr.capacity(); i++)
        {
            keys[i] = arr.insert("string" + std::to_string(i));
        }

        auto arr1 = arr;
        CHECK(arr1.size() == arr.size());
        for (llec::s32 i = 0; i < arr.size(); i++)
        {
            CHECK(arr[keys[i]] == arr1[keys[i]]);
        }
    }

    SECTION("Assignment")
    {
        llec::fop<std::string, 5> arr;

        typename decltype(arr)::handle keys[5];
        for (llec::s32 i = 0; i < arr.capacity(); i++)
        {
            keys[i] = arr.insert("string" + std::to_string(i));
        }

        decltype(arr) arr1;
        arr1 = arr;
        CHECK(arr1.size() == arr.size());
        for (llec::s32 i = 0; i < arr.size(); i++)
        {
            CHECK(arr[keys[i]] == arr1[keys[i]]);
        }
    }
}

TEST_CASE("Move operation", "[fixed_object_pool]")
{
    SECTION("Construction")
    {
        llec::fop<std::string, 5> arr;

        typename decltype(arr)::handle keys[5];
        for (llec::s32 i = 0; i < arr.capacity(); i++)
        {
            keys[i] = arr.insert("string" + std::to_string(i));
        }

        auto arr1 = std::move(arr);
        CHECK(arr1.size() == arr1.capacity());
        for (llec::s32 i = 0; i < arr.size(); i++)
        {
            CHECK(arr1[keys[i]] == std::string("string" + std::to_string(i)));
        }
    }

    SECTION("Assignment")
    {
        llec::fop<std::string, 5> arr;

        typename decltype(arr)::handle keys[5];
        for (llec::s32 i = 0; i < arr.capacity(); i++)
        {
            keys[i] = arr.insert("string" + std::to_string(i));
        }

        CHECK(arr.erase(keys[4]));
        CHECK(!arr.erase(keys[4]));
        CHECK(arr.erase(keys[0]));

        decltype(arr) arr1;
        arr1 = std::move(arr);
        CHECK(arr1.erase(keys[2]));
        CHECK(arr1.size() == 2);

        bool b{};
        for (auto&& elem : arr1)
        {
            if (!b)
            {
                CHECK(elem == "string3");
                b = true;
            }
            else
            {
                CHECK(elem == "string1");
            }
        }
    }
}

TEST_CASE("Subscript", "[fixed_object_pool]")
{
    llec::fop<std::string, 5> arr;

    typename decltype(arr)::handle keys[5];
    for (llec::s32 i = 0; i < arr.capacity(); i++)
    {
        keys[i] = arr.insert("string" + std::to_string(i));
    }
    CHECK(arr.size() == arr.capacity());
    CHECK(arr.erase(keys[4]));
    CHECK(!arr.erase(keys[4]));
    CHECK(arr.erase(keys[0]));
    CHECK(arr.erase(keys[2]));
    CHECK(!arr.is_handle_valid(keys[2]));
    CHECK(arr.size() == 2);
    CHECK(arr.is_handle_valid(keys[1]));
    CHECK(arr[keys[1]] == "string1");
}

TEST_CASE("Insert", "[fixed_object_pool][trivial]")
{
    llec::fop<llec::s32, 5> arr;

    typename decltype(arr)::handle keys[5];
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

TEST_CASE("Iterators", "[fixed_object_pool][trivial]")
{
    SECTION("Empty array")
    {
        const llec::fop<llec::s32, 10> arr;

        llec::s32 count{};
        for ([[maybe_unused]] auto&& elem : arr)
        {
            count++;
        }
        CHECK(count == 0);
    }

    SECTION("Full array")
    {
        llec::fop<llec::s32, 10> arr;

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

TEST_CASE("Data", "[fixed_object_pool][trivial]")
{
    llec::fop<llec::s32, 10> arr;

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

TEST_CASE("Clear", "[fixed_object_pool][trivial]")
{
    llec::fop<llec::s32, 10> arr;

    for (llec::s32 i = 0; i < arr.capacity(); i++)
    {
        arr.insert(i);
    }

    CHECK(arr.size() == arr.capacity());

    arr.clear();

    CHECK(arr.size() == 0);
}

TEST_CASE("Erase", "[fixed_object_pool][trivial]")
{
    llec::fop<llec::s32, 5> arr;

    typename decltype(arr)::handle keys[5];
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

TEST_CASE("Subscript", "[fixed_object_pool][trivial]")
{
    llec::fop<llec::s32, 5> arr;

    typename decltype(arr)::handle keys[5];
    for (llec::s32 i = 0; i < arr.capacity(); i++)
    {
        keys[i] = arr.insert(i);
    }

    CHECK(arr.size() == arr.capacity());
    CHECK(arr.erase(keys[4]));
    CHECK(!arr.erase(keys[4]));
    CHECK(arr.erase(keys[0]));
    CHECK(arr.erase(keys[2]));
    CHECK(!arr.is_handle_valid(keys[2]));
    CHECK(arr.size() == 2);
    CHECK(arr.is_handle_valid(keys[1]));
    CHECK(arr[keys[1]] == 1);
}

TEST_CASE("Move operation", "[fixed_object_pool][trivial]")
{
    SECTION("Construction")
    {
        llec::fop<llec::s32, 5> arr;

        typename decltype(arr)::handle keys[5];
        for (llec::s32 i = 0; i < arr.capacity(); i++)
        {
            keys[i] = arr.insert(i);
        }

        auto arr1 = std::move(arr);
        CHECK(arr1.size() == arr1.capacity());
        for (llec::s32 i = 0; i < arr.size(); i++)
        {
            CHECK(arr1[keys[i]] == i);
        }
    }

    SECTION("Assignment")
    {
        llec::fop<llec::s32, 5> arr;

        typename decltype(arr)::handle keys[5];
        for (llec::s32 i = 0; i < arr.capacity(); i++)
        {
            keys[i] = arr.insert(i);
        }

        CHECK(arr.erase(keys[4]));
        CHECK(!arr.erase(keys[4]));
        CHECK(arr.erase(keys[0]));

        decltype(arr) arr1;
        arr1 = std::move(arr);
        CHECK(arr1.erase(keys[2]));
        CHECK(arr1.size() == 2);

        bool b{};
        for (auto&& elem : arr1)
        {
            if (!b)
            {
                CHECK(elem == 3);
                b = true;
            }
            else
            {
                CHECK(elem == 1);
            }
        }
    }
}

TEST_CASE("Copy operation", "[fixed_object_pool][trivial]")
{
    SECTION("Construction")
    {
        llec::fop<llec::s32, 5> arr;

        typename decltype(arr)::handle keys[5];
        for (llec::s32 i = 0; i < arr.capacity(); i++)
        {
            keys[i] = arr.insert(i);
        }

        auto arr1 = arr;
        CHECK(arr1.size() == arr.size());
        for (llec::s32 i = 0; i < arr.size(); i++)
        {
            CHECK(arr[keys[i]] == arr1[keys[i]]);
        }
    }

    SECTION("Assignment")
    {
        llec::fop<llec::s32, 5> arr;

        typename decltype(arr)::handle keys[5];
        for (llec::s32 i = 0; i < arr.capacity(); i++)
        {
            keys[i] = arr.insert(i);
        }

        decltype(arr) arr1;
        arr1 = arr;
        CHECK(arr1.size() == arr.size());
        for (llec::s32 i = 0; i < arr.size(); i++)
        {
            CHECK(arr[keys[i]] == arr1[keys[i]]);
        }
    }
}
