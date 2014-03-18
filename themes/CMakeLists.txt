install(
    DIRECTORY
        Glass
    DESTINATION
        ${SHARE_INSTALL_PREFIX}/emoticons/
    FILES_MATCHING
        PATTERN "*.png"
        PATTERN "emoticons.xml"
)
