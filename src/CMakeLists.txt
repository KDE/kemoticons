
add_subdirectory(core)
add_subdirectory(providers)
add_subdirectory(integrationplugin)

ecm_qt_install_logging_categories(
    EXPORT KEMOTICONS
    FILE kemoticons.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)
