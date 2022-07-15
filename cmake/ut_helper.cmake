# CMake function to imitate Bazel's cc_test rule.
#
# Parameters:
# NAME: name of target (see Usage below)
# SRCS: List of source files for the binary
# DEPS: List of other libraries to be linked in to the binary targets
#
# Note:
# By default, sim_test will always create a binary named sim_test_${NAME}.
# This will also add it to ctest list as sim_test_${NAME}.
#
# Usage:
# sim_test(
#   NAME
#     trader
#   SRCS
#     "trader_test1.cpp"
#     "trader_test2.cpp"
#   DEPS
#     gtest
#     gtest_main
# )

find_package(Threads REQUIRED)

include(GoogleTest)
function(sim_test)
  cmake_parse_arguments(SIM_TEST
    ""
    "NAME"
    "SRCS;DEPS"
    ${ARGN}
  )

  set(_NAME "sim_test_${SIM_TEST_NAME}")

  add_executable(${_NAME} "")
  target_sources(${_NAME} PRIVATE ${SIM_TEST_SRCS})
  target_link_libraries(${_NAME}
    PUBLIC ${SIM_TEST_DEPS}
  )
  # Add all test executable to the unittest target folder.
  set_property(TARGET ${_NAME} PROPERTY FOLDER ${CMAKE_CURRENT_BINARY_DIR})

  gtest_discover_tests(${_NAME})
endfunction()


# CMake function to create simple sim UTs
#
# Parameters:
# DIR_NAME: directory name of source/test cpp files
#
# Note:
# DIR_NAME shall exist in both src/ and test/
#
# Usage:
# default_sim_test(DIR_NAME latency)


function(default_sim_test)
  cmake_parse_arguments(DEFAULT_SIM_TEST
    ""
    "DIR_NAME"
    "EXTRA_SRCS"
    ${ARGN}
  )
  aux_source_directory("../src/${DEFAULT_SIM_TEST_DIR_NAME}" SRCS)
  aux_source_directory(${DEFAULT_SIM_TEST_DIR_NAME} TEST_SRCS)
  sim_test(
    NAME
      ${DEFAULT_SIM_TEST_DIR_NAME}
    SRCS
      ${SRCS}
      ${TEST_SRCS}
      ${DEFAULT_SIM_TEST_EXTRA_SRCS}
    DEPS
      gtest
      gtest_main
      ${CMAKE_THREAD_LIBS_INIT}
  )
endfunction()
