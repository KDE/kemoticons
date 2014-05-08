include(ECMAddTests)

find_package(Qt5Test REQUIRED)
macro(kemoticons_unit_tests)
   foreach(_testname ${ARGN})
      ecm_add_test(${_testname}.cpp TEST_NAME kemoticons-${_testname} LINK_LIBRARIES Qt5::Test KF5::Emoticons)
   endforeach()
endmacro()

kemoticons_unit_tests(kemoticontest)

target_compile_definitions(kemoticons-kemoticontest PRIVATE
    LOCAL_THEMES_DIR="${KEmoticons_SOURCE_DIR}/themes")

