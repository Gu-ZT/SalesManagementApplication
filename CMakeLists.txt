cmake_minimum_required(VERSION 3.5)

set(CMAKE_PREFIX_PATH $ENV{Qt})

project(SalesManagement VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES SalesManagement_zh_CN.ts)

# 设置SQL文件目录
set(SQL_DIR ${CMAKE_SOURCE_DIR}/sql)

# 使用GLOB命令匹配所有的SQL文件
file(GLOB SQL_FILES "${SQL_DIR}/*.sql" "${CMAKE_SOURCE_DIR}/*.sql")

# 设置UI文件目录
set(UI_DIR ${CMAKE_SOURCE_DIR}/ui)

# 使用GLOB命令匹配所有的UI文件
file(GLOB UI_FILES "${UI_DIR}/*.ui" "${CMAKE_SOURCE_DIR}/*.ui")

# 设置头文件目录
set(HEADER_DIR ${CMAKE_SOURCE_DIR}/include)

# 使用GLOB命令匹配所有的头文件
file(GLOB HEADER_FILES "${HEADER_DIR}/*.h" "${CMAKE_SOURCE_DIR}/*.h")

# 设置源文件目录
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

# 使用GLOB命令匹配所有的源文件
file(GLOB SOURCE_FILES "${SOURCE_DIR}/*.cpp" "${CMAKE_SOURCE_DIR}/*.cpp")

# 设置库文件目录
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)

# 使用GLOB命令匹配所有的源文件和头文件
file(GLOB LIB_FILES "${LIB_DIR}/*.h" "${LIB_DIR}/*.cpp" "${LIB_DIR}/*/*.h" "${LIB_DIR}/*/*.cpp")

list(REMOVE_ITEM SOURCE_FILES "${SOURCE_DIR}/reportSale.cpp")

# 设置资源文件目录
set(RESOURCE_DIR ${CMAKE_SOURCE_DIR}/resource)

# 使用GLOB命令匹配所有资源文件
file(GLOB RESOURCE_FILES
        "${RESOURCE_DIR}/*.qrc"
        "${RESOURCE_DIR}/*.png"
)

set(PROJECT_SOURCES
        ${UI_FILES}
        ${HEADER_FILES}
        ${SOURCE_FILES}
        ${LIB_FILES}
        ${TS_FILES}
        ${SQL_FILES}
)

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SalesManagement
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
    )
    # Define target properties for Android with Qt 6 as:
    #    set_property(TARGET SalesManagement APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
    #                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
    # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else ()
    if (ANDROID)
        add_library(SalesManagement SHARED
                ${PROJECT_SOURCES}
        )
        # Define properties for Android with Qt 5 after find_package() calls as:
        #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else ()
        add_executable(SalesManagement
                ${PROJECT_SOURCES}
        )
    endif ()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif ()

find_package(Qt6 REQUIRED COMPONENTS Sql)
target_link_libraries(SalesManagement PRIVATE Qt6::Sql)
find_package(Qt6 REQUIRED COMPONENTS Charts)
target_link_libraries(SalesManagement PRIVATE Qt6::Charts)
target_link_libraries(SalesManagement PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if (${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SalesManagement)
endif ()
set_target_properties(SalesManagement PROPERTIES
        ${BUNDLE_ID_OPTION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS SalesManagement
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if (QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SalesManagement)
endif ()

include_directories(${CMAKE_SOURCE_DIR} ${UI_DIR} ${HEADER_DIR})
