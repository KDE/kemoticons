add_library(KF5Emoticons)
add_library(KF5::Emoticons ALIAS KF5Emoticons)

target_sources(KF5Emoticons PRIVATE
    kemoticons.cpp
    kemoticonstheme.cpp
    kemoticonsprovider.cpp
    kemoticonsglobal.cpp
)

ecm_qt_declare_logging_category(KF5Emoticons
    HEADER kemoticons_core_debug.h
    IDENTIFIER KEMOTICONS_CORE
    CATEGORY_NAME kf.emoticons
    OLD_CATEGORY_NAMES kf5.kemoticons.core
    DESCRIPTION "kemoticons core"
    EXPORT KEMOTICONS
)

ecm_generate_export_header(KF5Emoticons
    BASE_NAME KEmoticons
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    DEPRECATED_BASE_VERSION 0
    DEPRECATION_VERSIONS 5.0
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)

target_include_directories(KF5Emoticons INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/KEmoticons>")

target_link_libraries(KF5Emoticons
PUBLIC
    Qt5::Gui # for QImageReader, QPixmap
    KF5::Service     # for KService::Ptr, KServiceTypeTrader
PRIVATE
    KF5::Archive    # for KTar, KZip
    Qt5::DBus
)

set_target_properties(KF5Emoticons PROPERTIES
                      VERSION ${KEMOTICONS_VERSION}
                      SOVERSION ${KEMOTICONS_SOVERSION}
                      EXPORT_NAME Emoticons
)

install(FILES kemoticonsTheme.desktop DESTINATION ${KDE_INSTALL_KSERVICETYPES5DIR})

ecm_generate_headers(KEmoticons_HEADERS
  HEADER_NAMES
  KEmoticons
  KEmoticonsTheme
  KEmoticonsProvider

  REQUIRED_HEADERS KEmoticons_HEADERS
)


install(TARGETS KF5Emoticons EXPORT KF5EmoticonsTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kemoticons_export.h
  ${KEmoticons_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KEmoticons
  COMPONENT Devel
)

if(BUILD_QCH)
    ecm_add_qch(
        KF5Emoticons_QCH
        NAME KEmoticons
        BASE_NAME KF5Emoticons
        VERSION ${KF_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${KEmoticons_HEADERS}
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        LINK_QCHS
            Qt5Gui_QCH
            KF5Service_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            KEMOTICONS_EXPORT
            KEMOTICONS_DEPRECATED
            KEMOTICONS_DEPRECATED_EXPORT
            "KEMOTICONS_DEPRECATED_VERSION(x, y, t)"
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KEmoticons LIB_NAME KF5Emoticons DEPS "widgets KService" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/KEmoticons)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
