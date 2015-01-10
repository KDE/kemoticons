
########### next target ###############
set(adiumemoticons_SRCS adium_emoticons.cpp)

add_library(emoticonstheme_adium MODULE ${adiumemoticons_SRCS})

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
install(
    TARGETS emoticonstheme_adium
    DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/emoticonsthemes
)
install(
    FILES emoticonstheme_adium.desktop
    DESTINATION ${KDE_INSTALL_KSERVICES5DIR}
)
