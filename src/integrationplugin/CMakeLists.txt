add_library(KEmoticonsIntegrationPlugin MODULE)

target_sources(KEmoticonsIntegrationPlugin PRIVATE
    kemoticonsintegrationplugin.cpp
    kemoticonsintegrationplugin.h
    ktexttohtml.cpp
    ktexttohtml.h
)

target_link_libraries(KEmoticonsIntegrationPlugin
    PRIVATE
        KF5::CoreAddons
        KF5::Emoticons
)

install(TARGETS KEmoticonsIntegrationPlugin
        DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5)
