kcoreaddons_add_plugin(emoticonstheme_pidgin
    INSTALL_NAMESPACE kf5/emoticonsthemes
)

target_sources(emoticonstheme_pidgin PRIVATE pidgin_emoticons.cpp pidgin_emoticons.h)

ecm_qt_declare_logging_category(emoticonstheme_pidgin
    HEADER kemoticons_plugin_pidgin_debug.h
    IDENTIFIER KEMOTICONS_PLUGIN_PIDGIN
    CATEGORY_NAME kf.emoticons.pidgin
    OLD_CATEGORY_NAMES kf5.kemoticons.plugin_pidgin
    DESCRIPTION "kemoticons (plugin pidgin)"
    EXPORT KEMOTICONS
)

target_link_libraries(emoticonstheme_pidgin PRIVATE KF5::Emoticons)

########### install files ###############

set_target_properties(emoticonstheme_pidgin PROPERTIES
    OUTPUT_NAME "pidgin"
)
# KF6: remove
install(
    FILES emoticonstheme_pidgin.desktop
    DESTINATION ${KDE_INSTALL_KSERVICES5DIR}
)
