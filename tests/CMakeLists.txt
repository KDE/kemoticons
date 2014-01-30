find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Widgets)

add_executable(kemoticonstest main.cpp)

target_link_libraries(kemoticonstest KF5::Emoticons Qt5::Widgets)
