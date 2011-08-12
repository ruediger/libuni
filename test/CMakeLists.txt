if(BUILD_TESTS)

enable_testing()

include_directories(${libuni_SOURCE_DIR}/src)
file(GLOB Tests "test_*.c++")

foreach(Test_Source ${Tests})
  string(REGEX MATCH "test_[a-zA-Z0-9_]*" Test_Out ${Test_Source})
  string(REGEX REPLACE "^test_" "" Test_Name ${Test_Out})
  add_executable(${Test_Out} ${Test_Source})
  add_test(${Test_Name} ${EXECUTABLE_OUTPUT_PATH}/${Test_Out})
  set_property(
    TARGET ${Test_Out}
    PROPERTY COMPILE_FLAGS
    "-DBOOST_TEST_DYN_LINK -DBOOST_TEST_MODULE=${TEST_NAME}")
  target_link_libraries(
    ${Test_Out}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    uni)
endforeach()

endif()
