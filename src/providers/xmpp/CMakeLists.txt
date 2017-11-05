
########### next target ###############
set(xmppemoticons_SRCS xmpp_emoticons.cpp)

kcoreaddons_add_plugin(emoticonstheme_xmpp INSTALL_NAMESPACE kf5/emoticonsthemes JSON emoticonstheme_xmpp.json SOURCES ${xmppemoticons_SRCS})

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

# KF6: remove
install(
    FILES emoticonstheme_xmpp.desktop
    DESTINATION ${KDE_INSTALL_KSERVICES5DIR}
)
