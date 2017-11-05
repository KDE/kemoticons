
########### next target ###############
set(kdeemoticons_SRCS kde_emoticons.cpp)

kcoreaddons_add_plugin(emoticonstheme_kde INSTALL_NAMESPACE kf5/emoticonsthemes JSON emoticonstheme_kde.json SOURCES ${kdeemoticons_SRCS})

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
