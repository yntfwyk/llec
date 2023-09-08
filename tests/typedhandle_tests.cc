#include <catch2/catch_test_macros.hpp>
#include <core/typed_handle.hpp>

TEST_CASE("TypedHandle", "[typed_handle]")
{
    struct Vec3
    {
        Vec3 operator+(const Vec3& other) const
        {
            return Vec3{x + other.x, y + other.y, z + other.z};
        }
        Vec3 operator-(const Vec3& other) const
        {
            return Vec3{x - other.x, y - other.y, z - other.z};
        }
        float x;
        float y;
        float z;
    };

    llec::typed_handle<Vec3, struct test> typedVec1{{1, 2, 3}};
    llec::typed_handle<Vec3, struct test> typedVec2{{4, 5, 6}};

    auto result1 = typedVec1 + typedVec2;
    auto result2 = typedVec2 - typedVec1;

    CHECK(result1.data().x == 5.f);
    CHECK(result1.data().y == 7.f);
    CHECK(result1.data().z == 9.f);

    CHECK(result2.data().x == 3.f);
    CHECK(result2.data().y == 3.f);
    CHECK(result2.data().z == 3.f);
}
