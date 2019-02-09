set(MHZ19B_TEST_SRCS
    MockSerialInterface.h
    tests.cpp
    main.cpp
)

add_executable(MHZ19B_UNIT_TESTS ${MHZ19B_TEST_SRCS})
target_link_libraries(MHZ19B_UNIT_TESTS gtest_main gmock_main MHZ19B)

include(GoogleTest)

gtest_add_tests(TARGET MHZ19B_UNIT_TESTS)
