add_subdirectory(proto)

qt_add_executable(${GT_APP_NAME}
  main.cpp
)

gt_register_qml_mod(
  ROOT
  TARGET ${GT_APP_NAME}
  QML_FILES
  Main.qml
)

message("Building ${GT_APP_NAME} version ${PROJECT_VERSION}")

set_target_properties(${GT_APP_NAME} PROPERTIES
  MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
  MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
  MACOSX_BUNDLE TRUE
  WIN32_EXECUTABLE TRUE
)

add_subdirectory(qml)

target_link_libraries(${GT_APP_NAME}
  PRIVATE
  Qt6::Quick
  Gentau::Conf
  Gentau::App::Msg
  Gentau::App::Singleton
)

include(GNUInstallDirs)
install(TARGETS ${GT_APP_NAME}
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
