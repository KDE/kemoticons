
########### next target ###############
set(adiumemoticons_SRCS adium_emoticons.cpp)

ecm_qt_declare_logging_category(adiumemoticons_SRCS
    HEADER kemoticons_plugin_adium_debug.h
    IDENTIFIER KEMOTICONS_PLUGIN_ADIUM
    CATEGORY_NAME kf5.kemoticons.plugin_adium
    DESCRIPTION "kemoticons (plugin adium)"
    EXPORT KEMOTICONS
)

kcoreaddons_add_plugin(emoticonstheme_adium INSTALL_NAMESPACE kf5/emoticonsthemes JSON emoticonstheme_adium.json SOURCES ${adiumemoticons_SRCS})

find_package(Qt5Xml REQUIRED)
target_link_libraries(emoticonstheme_adium
PRIVATE
    KF5::Emoticons
    Qt5::Xml # for QDomDocument
)

########### install files ###############

set_target_properties(emoticonstheme_adium PROPERTIES
    OUTPUT_NAME "adium"
)
# KF6: remove
install(
    FILES emoticonstheme_adium.desktop
    DESTINATION ${KDE_INSTALL_KSERVICES5DIR}
)
