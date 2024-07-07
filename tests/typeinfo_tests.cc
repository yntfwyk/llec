#include <catch2/catch_test_macros.hpp>
#include "utility/typeinfo.hpp"

class TestClass
{
};
struct ChildClass : public TestClass
{
};

TEST_CASE("Type Hash", "[typeinfo]")
{
    using namespace llec::literals;
#if defined(LLEC_COMPILER_MSVC)
    STATIC_CHECK(llec::typeinfo::type_hash32<ChildClass>() == "struct ChildClass"_fnv32);
    STATIC_CHECK(llec::typeinfo::type_hash64<TestClass>() == "class TestClass"_fnv64);
    STATIC_CHECK(llec::typeinfo::type_hash64<int>() == "int"_fnv64);
#else
    STATIC_CHECK(llec::typeinfo::type_hash32<ChildClass>() == "ChildClass"_fnv32);
    STATIC_CHECK(llec::typeinfo::type_hash64<TestClass>() == "TestClass"_fnv64);
    STATIC_CHECK(llec::typeinfo::type_hash64<int>() == "int"_fnv64);
#endif
}