# All test .cpp files are compiled into a single test executable.
# doctest provides the shared main() (see test_main.cpp), so each test file
# only contributes TEST_CASEs. Drop a new *.cpp here and it is picked up
# automatically -- no need to edit this file.
enable_testing()
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(animengine_tests ${TEST_SOURCES})
target_include_directories(animengine_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(animengine_tests PRIVATE animengine)

add_test(NAME animengine_tests COMMAND animengine_tests)
