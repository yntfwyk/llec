#include <catch2/catch_test_macros.hpp>
#include <container/fixed_vector.hpp>
#include <iostream>
#include <string.h>

static std::string s_testString = "The quick brown fox jumps over the lazy dog";

TEST_CASE("Push Back", "[fixed_vector]")
{
    llec::fixed_vector<std::string, 5> vec;

    for (llec::s32 i = 0; i < vec.capacity(); i++)
    {
        vec.push_back(std::to_string(i));
    }

    CHECK(vec.size() == vec.capacity());
}

TEST_CASE("Pop Back", "[fixed_vector]")
{
    llec::fixed_vector<std::string, 5> vec;

    for (llec::s32 i = 0; i < vec.capacity(); i++)
    {
        vec.push_back(s_testString);
    }

    vec.pop_back();
    vec.pop_back();

    CHECK(vec.size() == 3);
}

TEST_CASE("Push Back", "[fixed_vector][trivial]")
{
    llec::fixed_vector<int, 5> vec;

    for (llec::s32 i = 0; i < vec.capacity(); i++)
    {
        vec.push_back(i);
    }

    CHECK(vec.size() == vec.capacity());
}

TEST_CASE("Pop Back", "[fixed_vector][trivial]")
{
    llec::fixed_vector<int, 5> vec;

    for (llec::s32 i = 0; i < vec.capacity(); i++)
    {
        vec.push_back(i);
    }

    vec.pop_back();
    vec.pop_back();

    CHECK(vec.size() == 3);
}

TEST_CASE("Iterator", "[fixed_vector][trivial]")
{
    llec::fixed_vector<int, 5> vec;

    for (llec::s32 i = 0; i < vec.capacity(); i++)
    {
        vec.push_back(i);
    }

    SECTION("Non-const")
    {
        llec::s32 i = 0;
        for ([[maybe_unused]] auto&& elem : vec)
        {
            i++;
        }
        CHECK(vec.size() == i);
        CHECK(vec.size() == (vec.cend() - vec.cbegin()));
    }

    SECTION("Const")
    {
        llec::s32 i = 0;
        for ([[maybe_unused]] auto&& elem : std::as_const(vec))
        {
            i++;
        }
        CHECK(vec.size() == i);
        CHECK(vec.size() == (vec.cend() - vec.cbegin()));
    }
}

TEST_CASE("Iterator", "[fixed_vector]")
{
    llec::fixed_vector<std::string, 5> vec;

    for (llec::s32 i = 0; i < vec.capacity(); i++)
    {
        vec.push_back(s_testString + std::to_string(i + 1));
    }

    SECTION("Non-const")
    {
        llec::s32 i = 0;
        for ([[maybe_unused]] auto&& elem : vec)
        {
            i++;
        }
        CHECK(vec.size() == i);
        CHECK(vec.size() == (vec.end() - vec.begin()));
    }

    SECTION("Const")
    {
        llec::s32 i = 0;
        for ([[maybe_unused]] auto&& elem : std::as_const(vec))
        {
            i++;
        }
        CHECK(vec.size() == i);
        CHECK(vec.size() == (vec.cend() - vec.cbegin()));
    }
}

TEST_CASE("Erase", "[fixed_vector]")
{
    SECTION("Random")
    {
        llec::fixed_vector<std::string, 5> vec;
        for (llec::s32 i = 0; i < vec.capacity(); i++)
        {
            vec.push_back(s_testString + std::to_string(i + 1));
        }

        CHECK(vec.size() == (vec.end() - vec.begin()));

        vec.erase(vec.end() - 1);
        vec.erase(vec.begin());
        CHECK(*vec.begin() == (s_testString + std::to_string(2)));
    }

    SECTION("Clear")
    {
        llec::fixed_vector<std::string, 5> vec;
        for (llec::s32 i = 0; i < vec.capacity(); i++)
        {
            vec.push_back(s_testString + std::to_string(i + 1));
        }

        CHECK(vec.size() == (vec.end() - vec.begin()));
        for (llec::s32 i = 0; i < vec.capacity(); i++)
        {
            vec.erase(vec.begin());
        }
        CHECK(vec.size() == 0);
    }
}

TEST_CASE("Erase", "[fixed_vector][trivial]")
{
    llec::fixed_vector<int, 11> vec;
    for (llec::s32 i = 0; i < vec.capacity(); i++)
    {
        vec.push_back(i);
    }

    auto&& iter = vec.begin();
    while (iter != vec.end())
    {
        if (*iter % 2 == 0)
        {
            iter = vec.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    CHECK(vec.size() == (vec.end() - vec.begin()));
    vec.erase(vec.end() - 1);
    CHECK(vec.size() == 4);
}

TEST_CASE("Range erase", "[fixed_vector][trivial]")
{
    llec::fixed_vector<int, 10> vec{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    vec.erase(vec.begin());
    vec.erase(vec.begin(), vec.end() - 1);
    for (auto&& it = vec.begin(); it != vec.end();)
    {
        if (*it % 2 == 0)
        {
            it = vec.erase(it);
        }
        else
        {
            ++it;
        }
    }

    CHECK(*(vec.cend() - 1) == 9);
    CHECK(vec.size() == (vec.end() - vec.begin()));
}

TEST_CASE("Range erase", "[fixed_vector]")
{
    llec::fixed_vector<std::string, 10> vec{s_testString + std::to_string(0), s_testString + std::to_string(1),
                                             s_testString + std::to_string(2), s_testString + std::to_string(3),
                                             s_testString + std::to_string(4)};

    vec.erase(vec.begin());
    vec.erase(vec.begin(), vec.end() - 1);
    CHECK((*(vec.cend() - 1))[s_testString.length()] == '4');
    CHECK(vec.size() == (vec.end() - vec.begin()));
}

TEST_CASE("Insert", "[fixed_vector]")
{
    llec::fixed_vector<std::string, 5> vec;
    for (llec::s32 i = 0; i < vec.capacity() - 3; i++)
    {
        vec.insert(vec.begin(), std::to_string(i));
    }
    
    vec.insert(vec.end(), std::to_string(4));
    vec.insert(vec.begin(), std::to_string(2));
    vec.insert(vec.begin() + 2, std::to_string(3));
    CHECK(vec.size() == (vec.end() - vec.begin()));
    CHECK(vec.size() == 5);
    CHECK(*(vec.end() - 1) == "4");
    CHECK(*(vec.end() - 3) == "3");
    CHECK(*vec.begin() == "2");
}

TEST_CASE("Insert", "[fixed_vector][trivial]")
{
    llec::fixed_vector<int, 5> vec;
    for (llec::s32 i = 0; i < vec.capacity() - 3; i++)
    {
        vec.insert(vec.end(), i);
    }

    vec.insert(vec.end(), 4);
    vec.insert(vec.begin(), 2);
    vec.insert(vec.begin() + 2, 3);
    
    CHECK(vec.size() == (vec.end() - vec.begin()));
    CHECK(vec.size() == 5);
    CHECK(*(vec.end() - 1) == 4);
    CHECK(*(vec.end() - 3) == 3);
    CHECK(*vec.begin() == 2);
}

TEST_CASE("Insert range", "[fixed_vector][trivial]")
{
    SECTION("Empty vector")
    {
        llec::fixed_vector<int, 5> vec;
        std::initializer_list<int> il = {1, 2, 3};
        vec.insert(vec.begin(), il.begin(), il.end());

        CHECK(vec.size() == 3);
    }

    SECTION("Range insert middle")
    {
        llec::fixed_vector<int, 10> vec;
        std::initializer_list<int> il = {1, 2, 3, 4, 5};
        vec.insert(vec.begin(), il.begin(), il.end());
        std::initializer_list<int> il1 = {6, 7, 8, 9, 10};
        vec.insert(vec.begin() + 3, il1.begin(), il1.end());

        CHECK(vec.size() == 10);
        CHECK(*(vec.end() - 1) == 5);
    }

    SECTION("Range insert end")
    {
        llec::fixed_vector<int, 10> vec;
        std::initializer_list<int> il = {1, 2, 3, 4, 5};
        vec.insert(vec.begin(), il.begin(), il.end());
        std::initializer_list<int> il1 = {6, 7, 8, 9, 10};
        vec.insert(vec.end(), il1.begin(), il1.end());

        CHECK(vec.size() == 10);
        CHECK(*(vec.end() - 1) == 10);
    }
}

TEST_CASE("Insert range", "[fixed_vector]")
{
    SECTION("Empty vector")
    {
        llec::fixed_vector<std::string, 5> vec;
        std::initializer_list<std::string> il = {"1", "2", "3"};
        vec.insert(vec.begin(), il.begin(), il.end());

        CHECK(vec.size() == 3);
        CHECK(vec.size() == (vec.end() - vec.begin()));
    }

    SECTION("Range insert middle")
    {
        llec::fixed_vector<std::string, 10> vec;
        std::initializer_list<std::string> il = {"1", "2", "3", "4", "5"};
        vec.insert(vec.begin(), il.begin(), il.end());
        std::initializer_list<std::string> il1 = {"6", "7", "8", "9", "10"};
        vec.insert(vec.begin() + 3, il1.begin(), il1.end());

        CHECK(vec.size() == 10);
        CHECK(*(vec.end() - 1) == "5");
        CHECK(vec.size() == (vec.end() - vec.begin()));
    }

    SECTION("Range insert end")
    {
        llec::fixed_vector<std::string, 10> vec;
        std::initializer_list<std::string> il = {"1", "2", "3", "4", "5"};
        vec.insert(vec.begin(), il.begin(), il.end());
        std::initializer_list<std::string> il1 = {"6", "7", "8", "9", "10"};
        vec.insert(vec.end(), il1.begin(), il1.end());

        CHECK(vec.size() == 10);
        CHECK(*(vec.end() - 1) == "10");
        llec::s32 i = 0;
        for (auto&& elem : vec)
        {
            CHECK(elem == std::to_string(++i));
        }
        CHECK(vec.size() == (vec.end() - vec.begin()));
    }
}

TEST_CASE("Initializer list", "[fixed_vector][trivial]")
{
    constexpr llec::fixed_vector<int, 5> cvec{1, 2, 3, 4, 5};
    CHECK(cvec.size() == 5);
    llec::fixed_vector<int, 10> vec{6, 7, 8, 9, 10};
    vec.insert(vec.begin(), cvec.begin(), cvec.end());
    for (llec::s32 i = 0; i < vec.capacity(); i++)
    {
        CHECK(*(vec.cbegin() + i) == (i + 1));
    }

    CHECK(vec.size() == (vec.end() - vec.begin()));
    CHECK(vec.size() == (cvec.size() * 2));
}

TEST_CASE("Initializer list", "[fixed_vector]")
{
    llec::fixed_vector<std::string, 5> vec{"1", "2", "3", "4", "5"};
    CHECK(vec.size() == 5);

    for (llec::s32 i = 0; i < 5; i++)
    {
        CHECK(*(vec.cbegin() + i) == std::to_string(i + 1));
    }

    CHECK(vec.size() == (vec.end() - vec.begin()));
}

TEST_CASE("Emplace", "[fixed_vector][trivial]")
{
    llec::fixed_vector<int, 5> vec{6, 7, 8, 9};
    vec.emplace(vec.end(), 10);
    CHECK(vec.size() == vec.capacity());
    CHECK(*(vec.end() - 1) == 10);
    CHECK(vec.size() == (vec.end() - vec.begin()));
}

TEST_CASE("Emplace", "[fixed_vector]")
{
    llec::fixed_vector<std::string, 5> vec{"6", "7", "8", "9"};
    vec.emplace(vec.end(), "10");
    CHECK(vec.size() == vec.capacity());
    CHECK(*(vec.end() - 1) == "10");
}

TEST_CASE("Emplace back", "[fixed_vector][trivial]")
{
    llec::fixed_vector<int, 5> vec;
    for (llec::s32 i = 0; i < vec.capacity(); i++)
    {
        vec.emplace_back(i);
    }
    CHECK(vec.size() == vec.capacity());

    llec::s32 n{};
    for (auto&& elem : vec)
    {
        CHECK(elem == n++);
    }

    CHECK(vec[vec.size() - 1] == 4);
    CHECK(vec.size() == (vec.end() - vec.begin()));
}

TEST_CASE("Emplace back", "[fixed_vector]")
{
    llec::fixed_vector<std::string, 5> vec;
    for (llec::s32 i = 0; i < vec.capacity(); i++)
    {
        vec.emplace_back(std::to_string(i));
    }
    CHECK(vec.size() == vec.capacity());

    llec::s32 n{};
    for (auto&& elem : vec)
    {
        CHECK(elem == std::to_string(n++));
    }

    CHECK(vec[vec.size() - 1] == "4");
    CHECK(vec.size() == (vec.end() - vec.begin()));
}

TEST_CASE("Clear", "[fixed_vector]")
{
    llec::fixed_vector<std::string, 5> vec;
    for (llec::s32 i = 0; i < vec.capacity(); i++)
    {
        vec.emplace_back(std::to_string(i));
    }
    vec.clear();
    CHECK(vec.size() == 0);
}

TEST_CASE("Move operations", "[fixed_vector]")
{
    SECTION("Move constructor")
    {
        llec::fixed_vector<std::string, 5> vec = llec::fixed_vector<std::string, 5>{"1", "2", "3", "4", "5"};
        llec::s32 i{1};
        for (auto&& elem : vec)
        {
            CHECK(elem == std::to_string(i++));
        }
        CHECK(vec.size() == vec.capacity());
    }

    SECTION("Move assignment")
    {
        llec::fixed_vector<std::string, 5> vec;
        for (llec::s32 i = 0; i < vec.capacity(); i++)
        {
            vec.emplace_back(std::to_string(i));
        }
        decltype(vec) vec1 = std::move(vec);
        llec::s32 i{};
        for (auto&& elem : vec1)
        {
            CHECK(elem == std::to_string(i++));
        }
        CHECK(vec1.size() == vec1.capacity());
    }
}