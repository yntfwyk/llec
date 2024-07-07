#include <catch2/catch_test_macros.hpp>
#include "container/fused_vector.hpp"

template <size_t N>
struct fv_fixed_vector
{
    using container_tag = llec::fused_vector_container_tag;
    // Note: container_type must have a single template parameter
    template <typename T>
    using container_type = llec::fixed_vector<T, N>;
};

struct fv_std_vector
{
    using container_tag = llec::fused_vector_container_tag;
    // Note: container_type must have a single template parameter
    template <typename T>
    using container_type = std::vector<T>;
};

TEST_CASE("Push Back", "[fused_vector]")
{
    SECTION("Fixed Vector")
    {
        llec::fused_vector<fv_fixed_vector<5>, int, float> fusedWrapper;
        fusedWrapper.push_back(1, 2.f);
        fusedWrapper.push_back(4.f, 3);
        fusedWrapper.push_back(5.f, 6);
        fusedWrapper.push_back(7, 8.f);
        fusedWrapper.push_back(9, 10.f);
        CHECK(fusedWrapper.size() == 5);
    }

    SECTION("Standard Vector")
    {
        llec::fused_vector<fv_std_vector, int, float> fusedWrapper;
        fusedWrapper.push_back(1, 2.f);
        fusedWrapper.push_back(4.f, 3);
        fusedWrapper.push_back(5.f, 6);
        fusedWrapper.push_back(7, 8.f);
        fusedWrapper.push_back(9, 10.f);
        CHECK(fusedWrapper.size() == 5);
    }
}

TEST_CASE("Clear", "[fused_vector]")
{
    SECTION("Fixed Vector")
    {
        llec::fused_vector<fv_fixed_vector<5>, int, float> fusedWrapper;
        fusedWrapper.push_back(1, 2.f);
        fusedWrapper.push_back(4.f, 3);
        fusedWrapper.push_back(5.f, 6);
        fusedWrapper.push_back(7, 8.f);
        fusedWrapper.push_back(9, 10.f);
        fusedWrapper.clear();
        CHECK(fusedWrapper.size() == 0);
    }

    SECTION("Standard Vector")
    {
        llec::fused_vector<fv_std_vector, int, float> fusedWrapper;
        fusedWrapper.push_back(1, 2.f);
        fusedWrapper.push_back(4.f, 3);
        fusedWrapper.push_back(5.f, 6);
        fusedWrapper.push_back(7, 8.f);
        fusedWrapper.push_back(9, 10.f);
        fusedWrapper.clear();
        CHECK(fusedWrapper.size() == 0);
    }
}

TEST_CASE("Erase", "[fused_vector]")
{
    SECTION("Fixed Vector")
    {
        llec::fused_vector<fv_fixed_vector<5>, int, float> fusedWrapper;
        fusedWrapper.push_back(1, 2.f);
        fusedWrapper.push_back(4.f, 3);
        fusedWrapper.push_back(5.f, 6);
        fusedWrapper.push_back(7, 8.f);
        fusedWrapper.push_back(9, 10.f);
        fusedWrapper.erase(3);
        fusedWrapper.erase(0);
        CHECK(fusedWrapper.size() == 3);
        CHECK(fusedWrapper.get_component_at<int>(0) == 3);
        CHECK(fusedWrapper.get_component_at<float>(2) == 10.f);
    }

    SECTION("Standard Vector")
    {
        llec::fused_vector<fv_std_vector, int, float> fusedWrapper;
        fusedWrapper.push_back(1, 2.f);
        fusedWrapper.push_back(4.f, 3);
        fusedWrapper.push_back(5.f, 6);
        fusedWrapper.push_back(7, 8.f);
        fusedWrapper.push_back(9, 10.f);
        fusedWrapper.erase(3);
        fusedWrapper.erase(0);
        CHECK(fusedWrapper.size() == 3);
        CHECK(fusedWrapper.get_component_at<int>(0) == 3);
        CHECK(fusedWrapper.get_component_at<float>(2) == 10.f);
    }
}

TEST_CASE("Range Erase", "[fused_vector]")
{
    SECTION("Fixed Vector")
    {
        llec::fused_vector<fv_fixed_vector<5>, int, float> fusedWrapper;
        fusedWrapper.push_back(1, 2.f);
        fusedWrapper.push_back(4.f, 3);
        fusedWrapper.push_back(5.f, 6);
        fusedWrapper.push_back(7, 8.f);
        fusedWrapper.push_back(9, 10.f);
        fusedWrapper.erase(1, 3);
        CHECK(fusedWrapper.size() == 3);
        CHECK(fusedWrapper.get_component_at<int>(0) == 1);
        CHECK(fusedWrapper.get_component_at<float>(2) == 10.f);
    }

    SECTION("Standard Vector")
    {
        llec::fused_vector<fv_std_vector, int, float> fusedWrapper;
        fusedWrapper.push_back(1, 2.f);
        fusedWrapper.push_back(4.f, 3);
        fusedWrapper.push_back(5.f, 6);
        fusedWrapper.push_back(7, 8.f);
        fusedWrapper.push_back(9, 10.f);
        fusedWrapper.erase(1, 3);
        CHECK(fusedWrapper.size() == 3);
        CHECK(fusedWrapper.get_component_at<int>(0) == 1);
        CHECK(fusedWrapper.get_component_at<float>(2) == 10.f);
    }
}

TEST_CASE("View", "[fused_vector]")
{
    SECTION("Fixed Vector")
    {
        llec::fused_vector<fv_fixed_vector<5>, int, float> fusedWrapper;
        fusedWrapper.push_back(1, 1.f);
        fusedWrapper.push_back(2, 2.f);
        fusedWrapper.push_back(3, 3.f);
        fusedWrapper.push_back(4, 4.f);
        fusedWrapper.push_back(5, 5.f);
        CHECK(fusedWrapper.size() == 5);
        {
            llec::s32 i{1};
            for (auto&& elem : fusedWrapper.component_view<int>())
            {
                CHECK(elem == i++);
            }
        }
        {
            llec::f32 i{1};
            for (auto&& elem : fusedWrapper.component_view<float>())
            {
                CHECK(elem == i++);
            }
        }
    }

    SECTION("Standard Vector")
    {
        llec::fused_vector<fv_std_vector, int, float> fusedWrapper;
        fusedWrapper.push_back(1, 1.f);
        fusedWrapper.push_back(2, 2.f);
        fusedWrapper.push_back(3, 3.f);
        fusedWrapper.push_back(4, 4.f);
        fusedWrapper.push_back(5, 5.f);
        CHECK(fusedWrapper.size() == 5);
        {
            llec::s32 i{1};
            for (auto&& elem : fusedWrapper.component_view<int>())
            {
                CHECK(elem == i++);
            }
        }
        {
            llec::f32 i{1};
            for (auto&& elem : fusedWrapper.component_view<float>())
            {
                CHECK(elem == i++);
            }
        }
    }
}

TEST_CASE("Move", "[fused_vector]")
{
    SECTION("Fixed Vector")
    {
        llec::fused_vector<fv_fixed_vector<5>, int, float> fusedWrapper;
        fusedWrapper.push_back(1, 1.f);
        fusedWrapper.push_back(2, 2.f);
        fusedWrapper.push_back(3, 3.f);
        fusedWrapper.push_back(4, 4.f);
        fusedWrapper.push_back(5, 5.f);

        llec::fixed_vector<int, 5> tmp{5, 4, 3, 2, 1};
        fusedWrapper.move_component_data<int>(std::move(tmp));
        CHECK(fusedWrapper.size() == 5);
        {
            llec::s32 i{5};
            for (auto&& elem : fusedWrapper.component_view<int>())
            {
                CHECK(elem == i--);
            }
        }
    }

    SECTION("Standard Vector")
    {
        llec::fused_vector<fv_std_vector, int, float> fusedWrapper;
        fusedWrapper.push_back(1, 1.f);
        fusedWrapper.push_back(2, 2.f);
        fusedWrapper.push_back(3, 3.f);
        fusedWrapper.push_back(4, 4.f);
        fusedWrapper.push_back(5, 5.f);

        std::vector<float> tmp{5.f, 4.f, 3.f, 2.f, 1.f};
        fusedWrapper.move_component_data<float>(std::move(tmp));
        CHECK(fusedWrapper.size() == 5);
        {
            llec::f32 i{5.f};
            for (auto&& elem : fusedWrapper.component_view<float>())
            {
                CHECK(elem == i--);
            }
        }
    }
}

TEST_CASE("Copy", "[fused_vector]")
{
    SECTION("Fixed Vector")
    {
        llec::fused_vector<fv_fixed_vector<5>, int, float> fusedWrapper;
        fusedWrapper.push_back(1, 1.f);
        fusedWrapper.push_back(2, 2.f);
        fusedWrapper.push_back(3, 3.f);
        fusedWrapper.push_back(4, 4.f);
        fusedWrapper.push_back(5, 5.f);

        llec::fixed_vector<int, 5> tmp{5, 4, 3, 2, 1};
        fusedWrapper.copy_component_data<int>(tmp);
        CHECK(fusedWrapper.size() == 5);
        {
            llec::s32 i{5};
            for (auto&& elem : fusedWrapper.component_view<int>())
            {
                CHECK(elem == i--);
            }
        }
    }

    SECTION("Standard Vector")
    {
        llec::fused_vector<fv_std_vector, int, float> fusedWrapper;
        fusedWrapper.push_back(1, 1.f);
        fusedWrapper.push_back(2, 2.f);
        fusedWrapper.push_back(3, 3.f);
        fusedWrapper.push_back(4, 4.f);
        fusedWrapper.push_back(5, 5.f);

        std::vector<float> tmp{5.f, 4.f, 3.f, 2.f, 1.f};
        fusedWrapper.copy_component_data<float>(tmp);
        CHECK(fusedWrapper.size() == 5);
        {
            llec::f32 i{5.f};
            for (auto&& elem : fusedWrapper.component_view<float>())
            {
                CHECK(elem == i--);
            }
        }
    }
}
