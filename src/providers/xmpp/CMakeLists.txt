
########### next target ###############
set(xmppemoticons_SRCS xmpp_emoticons.cpp)

add_library(emoticonstheme_xmpp MODULE ${xmppemoticons_SRCS})

find_package(Qt5Xml REQUIRED)
target_link_libraries(emoticonstheme_xmpp
PRIVATE
    KF5::Emoticons
    Qt5::Xml # for QDomDocument
)

########### install files ###############

set_target_properties(emoticonstheme_xmpp PROPERTIES
    OUTPUT_NAME "xmpp"
)
install(
    TARGETS emoticonstheme_xmpp
    DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/emoticonsthemes
)
install(
    FILES emoticonstheme_xmpp.desktop
    DESTINATION ${KDE_INSTALL_KSERVICES5DIR}
)
