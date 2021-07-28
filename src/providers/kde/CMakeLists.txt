kcoreaddons_add_plugin(emoticonstheme_kde
    INSTALL_NAMESPACE kf5/emoticonsthemes
)

target_sources(emoticonstheme_kde PRIVATE kde_emoticons.cpp)

ecm_qt_declare_logging_category(emoticonstheme_kde
    HEADER kemoticons_plugin_kde_debug.h
    IDENTIFIER KEMOTICONS_PLUGIN_KDE
    CATEGORY_NAME kf.emoticons.kde
    OLD_CATEGORY_NAMES kf5.kemoticons.plugin_kde
    DESCRIPTION "kemoticons (plugin kde)"
    EXPORT KEMOTICONS
)

find_package(Qt5Xml REQUIRED)
target_link_libraries(emoticonstheme_kde
PRIVATE
    KF5::Emoticons
    Qt5::Xml # for QDomDocument
)
########### install files ###############

set_target_properties(emoticonstheme_kde PROPERTIES
    OUTPUT_NAME "kde"
)

# KF6: remove
install(
    FILES emoticonstheme_kde.desktop
    DESTINATION ${KDE_INSTALL_KSERVICES5DIR}
)
