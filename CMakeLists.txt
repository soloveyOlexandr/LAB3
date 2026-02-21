cmake_minimum_required(VERSION 3.19)
project(LAB3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#модуль GuiPrivate підключений спеціально для використання <private/qzipwriter_p.h>, який є внутрішнім (прихованим) класом Qt
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Gui GuiPrivate)

#автоматично налаштовує стандартні параметри
qt_standard_project_setup()

#вказує, які файли вихідного коду компілювати у виконуваний файл
qt_add_executable(LAB3
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
)

#Лінкує (зв'язує) знайдені бібліотеки Qt з нашим застосунком.
target_link_libraries(LAB3
    PRIVATE
        Qt6::Widgets
        Qt6::Gui
        Qt6::GuiPrivate
)

#include і install відповідають за правила встановлення програми
include(GNUInstallDirs)

install(TARGETS LAB3
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET LAB3
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
