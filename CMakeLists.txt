cmake_minimum_required(VERSION 3.5)

# vcpkg.cmake should be passed before the first project() call.
set(CMAKE_TOOLCHAIN_FILE "C:/src_vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
message("CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")

project(filmQA_netOD_normalizer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Gui WebEngineWidgets)

find_package(OpenCV REQUIRED)
find_package(TIFF REQUIRED)
message("TIFF_FOUND: ${TIFF_FOUND}")
message("TIFF_INCLUDE_DIR: ${TIFF_INCLUDE_DIR}")
message("TIFF_LIBRARY: ${TIFF_LIBRARY}")
message("TIFF_LIBRARIES: ${TIFF_LIBRARIES}")

set(TS_FILES filmQA_netOD_normalizer_ja_JP.ts)

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/mainwindow.ui
        src/my_graphics_view.h
        src/my_graphics_view.cpp
        src/my_graphics_view.ui
        src/cvmat_qimage_bridge.h
        src/cvmat_qimage_bridge.cpp
        src/process_step_widget.h
        src/process_step_widget.cpp
        src/process_step_widget.ui
        src/image_path_edit.h
        src/image_path_edit.cpp
        src/cv_mat_processor.h
        src/cv_mat_processor.cpp
        src/custom_types.h
        src/normalization_window.h
        src/normalization_window.cpp
        src/normalization_window.ui
        src/pushbutton_with_word_wrap.h
        src/pushbutton_with_word_wrap.cpp
        src/${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(filmQA_netOD_normalizer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET filmQA_netOD_normalizer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(filmQA_netOD_normalizer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(filmQA_netOD_normalizer
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(filmQA_netOD_normalizer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::WebEngineWidgets ${OpenCV_LIBS} ${TIFF_LIBRARIES})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.filmQA_netOD_normalizer)
endif()
set_target_properties(filmQA_netOD_normalizer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS filmQA_netOD_normalizer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(filmQA_netOD_normalizer)
endif()
