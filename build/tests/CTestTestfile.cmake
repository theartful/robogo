# CMake generated Testfile for 
# Source directory: /media/reem/Data/4th years projects/MI - Project/RoboGo/tests
# Build directory: /media/reem/Data/4th years projects/MI - Project/RoboGo/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test("goslayer-unit-tests:test sgf" "/media/reem/Data/4th years projects/MI - Project/RoboGo/build/bin/goslayer-unit-tests" "test sgf")
set_tests_properties("goslayer-unit-tests:test sgf" PROPERTIES  FAIL_REGULAR_EXPRESSION "No tests ran" LABELS "goslayer-unit-tests")
add_test("goslayer-unit-tests:Board updates" "/media/reem/Data/4th years projects/MI - Project/RoboGo/build/bin/goslayer-unit-tests" "Board updates")
set_tests_properties("goslayer-unit-tests:Board updates" PROPERTIES  FAIL_REGULAR_EXPRESSION "No tests ran" LABELS "goslayer-unit-tests")
add_test("goslayer-unit-tests:test 1" "/media/reem/Data/4th years projects/MI - Project/RoboGo/build/bin/goslayer-unit-tests" "test 1")
set_tests_properties("goslayer-unit-tests:test 1" PROPERTIES  FAIL_REGULAR_EXPRESSION "No tests ran" LABELS "goslayer-unit-tests;tags")
