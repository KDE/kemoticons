set(KEmoticonsIntegrationPlugin_SRCS
    kemoticonsintegrationplugin.cpp
    ktexttohtml.cpp
)

add_library(KEmoticonsIntegrationPlugin MODULE ${KEmoticonsIntegrationPlugin_SRCS})

target_link_libraries(KEmoticonsIntegrationPlugin
    PRIVATE
        KF5::CoreAddons
        KF5::Emoticons
)

install(TARGETS KEmoticonsIntegrationPlugin
        DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5)
