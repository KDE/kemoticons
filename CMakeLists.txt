cmake_minimum_required(VERSION 3.16)

set(KF_VERSION "5.90.0") # handled by release scripts
set(KF_DEP_VERSION "5.90.0") # handled by release scripts
project(KEmoticons VERSION ${KF_VERSION})

include(FeatureSummary)
find_package(ECM 5.90.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)


set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(REQUIRED_QT_VERSION 5.15.2)
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Gui DBus)

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

include(ECMGenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMAddQch)
include(CMakePackageConfigHelpers)
include(ECMQtDeclareLoggingCategory)

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

ecm_setup_version(PROJECT VARIABLE_PREFIX KEMOTICONS
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kemoticons_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5EmoticonsConfigVersion.cmake"
                        SOVERSION 5)

find_package(KF5Archive ${KF_DEP_VERSION} REQUIRED)
find_package(KF5Config ${KF_DEP_VERSION} REQUIRED)
find_package(KF5Service ${KF_DEP_VERSION} REQUIRED)
find_package(KF5CoreAddons ${KF_DEP_VERSION} REQUIRED)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kemoticons_version.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel )


# the strings throughout this library are used to search resources
# so they don't need translations
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050f02)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x055100)
add_subdirectory(src)
add_subdirectory(themes)
if (BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(tests)
endif()

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5Emoticons")

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KF5Emoticons_QCH
        FILE KF5EmoticonsQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KF5EmoticonsQchTargets.cmake\")")
endif()

install(EXPORT KF5EmoticonsTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5EmoticonsTargets.cmake NAMESPACE KF5:: )

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/KF5EmoticonsConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/KF5EmoticonsConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                             )

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/KF5EmoticonsConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/KF5EmoticonsConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
       )

feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)

