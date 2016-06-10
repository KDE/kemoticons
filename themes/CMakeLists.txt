install(
    DIRECTORY
        Breeze Konqi EmojiOne
    DESTINATION
        ${KDE_INSTALL_DATAROOTDIR}/emoticons/
    FILES_MATCHING
        PATTERN "*.png"
        PATTERN "emoticons.xml"
)
