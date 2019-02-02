cmake_minimum_required(VERSION 3.5)

set(KF5_VERSION "5.55.0") # handled by release scripts
set(KF5_DEP_VERSION "5.55.0") # handled by release scripts
project(KEmoticons VERSION ${KF5_VERSION})

include(FeatureSummary)
find_package(ECM 5.55.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)


set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(REQUIRED_QT_VERSION 5.10.0)
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Gui DBus)

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)


include(GenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMAddQch)
include(CMakePackageConfigHelpers)
include(ECMQtDeclareLoggingCategory)

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

ecm_setup_version(PROJECT VARIABLE_PREFIX KEMOTICONS
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kemoticons_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5EmoticonsConfigVersion.cmake"
                        SOVERSION 5)

find_package(KF5Archive ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5Config ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5Service ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5CoreAddons ${KF5_DEP_VERSION} REQUIRED)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kemoticons_version.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel )


# the strings throughout this library are used to search ressources
# so they don't need translations
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)
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

install( FILES kemoticons.categories DESTINATION ${KDE_INSTALL_CONFDIR} )

feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)

