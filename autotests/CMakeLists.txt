include(ECMAddTests)

find_package(Qt5Test REQUIRED)
macro(kemoticons_unit_test)
   set(_testname ${ARGV0})
   ecm_add_test(${_testname}.cpp ${ARGV1} TEST_NAME kemoticons-${_testname} LINK_LIBRARIES Qt5::Test KF5::Emoticons)
endmacro()

kemoticons_unit_test(kemoticontest)
kemoticons_unit_test(ktexttohtmlplugintest ../src/integrationplugin/ktexttohtml.cpp)

