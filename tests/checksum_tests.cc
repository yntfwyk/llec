#include <catch2/catch_test_macros.hpp>

#include <utility/checksum.hpp>

TEST_CASE("Adler32", "[checksum][adler32]")
{
    CHECK(llec::checksum::adler32("llec", 4u) == 0x042501A1u);

    llec::u32 number = 9u;
    CHECK(llec::checksum::adler32(&number, sizeof(llec::u32)) == 0x0028000Au);

    llec::f32 fnumber = 0.99556644f;
    CHECK(llec::checksum::adler32(&fnumber, sizeof(llec::f32)) == 0x059A020Cu);
}

TEST_CASE("FNV1a32", "[checksum][fnv1a]")
{
    CHECK(llec::checksum::fnv1a("llec", 4u) == 0x6EA3999u);

    llec::u32 number = 9u;
    CHECK(llec::checksum::fnv1a(&number, sizeof(llec::u32)) == 0x7CCBAE8Cu);

    llec::f32 fnumber = 0.99556644f;
    CHECK(llec::checksum::fnv1a(&fnumber, sizeof(llec::f32)) == 0x7DDCD278u);
}

TEST_CASE("FNV1a64", "[checksum][fnv1a]")
{
    CHECK(llec::checksum::fnv1a<llec::u64>("llec", 4u) == 0xEA8340AD81C318D9u);

    llec::u32 number = 9u;
    CHECK(llec::checksum::fnv1a<llec::u64>(&number, sizeof(llec::u32)) == 0xAD002AB9F9463BECu);

    llec::f32 fnumber = 0.99556644f;
    CHECK(llec::checksum::fnv1a<llec::u64>(&fnumber, sizeof(llec::f32)) == 0x976B0005B2C623D8u);
}
