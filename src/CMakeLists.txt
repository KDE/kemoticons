
add_subdirectory(core)
add_subdirectory(providers)

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME  LIB_NAME  DEPS "core" FILENAME_VAR PRI_FILENAME)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
