enable_testing()
include(../cmake/ut_helper.cmake)

add_library(${PROJECT_NAME}_header INTERFACE)
target_include_directories(${PROJECT_NAME}_header INTERFACE
    ${PROJECT_SOURCE_DIR}/third-party
)

sim_test(
  NAME
    node
  SRCS
    "./node/NodeTests.cpp"

    "../src/mock/MockNodes.cpp"
    "../src/node/Node.cpp"
  DEPS
    gtest
    gtest_main
)