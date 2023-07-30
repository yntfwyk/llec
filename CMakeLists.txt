cmake_minimum_required(VERSION 3.9.1)
project(llec)
set(CMAKE_CXX_STANDARD 20)

add_compile_definitions($<$<CONFIG:Debug>:LLEC_DEBUG_BUILD>)

set(LLEC_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/llec)
set(LLEC_SRC_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

set(LLEC_HEADERS ${LLEC_INC_DIR}/llec.hpp
    ${LLEC_INC_DIR}/core/core.hpp
    ${LLEC_INC_DIR}/core/time_capture.hpp
    ${LLEC_INC_DIR}/core/memory.hpp
    ${LLEC_INC_DIR}/container/fixed_vector.hpp
    ${LLEC_INC_DIR}/container/fixed_string.hpp
    ${LLEC_INC_DIR}/container/fixed_object_pool.hpp
    ${LLEC_INC_DIR}/container/static_unordered_map.hpp
    ${LLEC_INC_DIR}/container/small_unordered_set.hpp
    ${LLEC_INC_DIR}/math/fast_math.hpp
    ${LLEC_INC_DIR}/utility/checksum.hpp
    ${LLEC_INC_DIR}/private/aligned_storage.hpp
    ${LLEC_INC_DIR}/private/basic_iterator.hpp)

set(LLEC_SRC_TESTS ${LLEC_SRC_TESTS_DIR}/checksum_tests.cc
    ${LLEC_SRC_TESTS_DIR}/fixedvector_tests.cc
    ${LLEC_SRC_TESTS_DIR}/fixedstring_tests.cc
    ${LLEC_SRC_TESTS_DIR}/fixedobjectpool_tests.cc
    ${LLEC_SRC_TESTS_DIR}/smallunorderedset_tests.cc)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.3.2 # or a later release
)

FetchContent_MakeAvailable(Catch2)

add_executable(
  llec_tests
  ${LLEC_SRC_TESTS}
  ${LLEC_HEADERS}
  .clang-format
  README.md)

target_include_directories(llec_tests PRIVATE ${LLEC_INC_DIR})
target_link_libraries(llec_tests PRIVATE Catch2::Catch2WithMain)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT llec_tests)

target_link_libraries(
  llec_tests
)
