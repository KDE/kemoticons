
########### next target ###############
set(pidginemoticons_SRCS pidgin_emoticons.cpp)

add_library(emoticonstheme_pidgin MODULE ${pidginemoticons_SRCS})

target_link_libraries(emoticonstheme_pidgin PRIVATE KF5::Emoticons)

########### install files ###############

set_target_properties(emoticonstheme_pidgin PROPERTIES
    OUTPUT_NAME "pidgin"
)
install(
    TARGETS emoticonstheme_pidgin
    DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/emoticonsthemes
)
install(
    FILES emoticonstheme_pidgin.desktop
    DESTINATION ${KDE_INSTALL_KSERVICES5DIR}
)
