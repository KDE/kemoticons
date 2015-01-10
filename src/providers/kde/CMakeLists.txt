
########### next target ###############
set(kdeemoticons_SRCS kde_emoticons.cpp)

add_library(emoticonstheme_kde MODULE ${kdeemoticons_SRCS})

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
install(
    TARGETS emoticonstheme_kde
    DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/emoticonsthemes
)
install(
    FILES emoticonstheme_kde.desktop
    DESTINATION ${KDE_INSTALL_KSERVICES5DIR}
)
