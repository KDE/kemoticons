
########### next target ###############
set(pidginemoticons_SRCS pidgin_emoticons.cpp)

kcoreaddons_add_plugin(emoticonstheme_pidgin INSTALL_NAMESPACE kf5/emoticonsthemes JSON emoticonstheme_pidgin.json SOURCES ${pidginemoticons_SRCS})

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
