file(
  GLOB QML_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "*.qml"
)

file(
  GLOB IMG_FILES 
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "images/*"
)

gt_register_qml_mod(
  TARGET basic-widgets
  URI_SUFFIX BasicWidgets
  QML_FILES
    ${QML_FILES}
  QT_ARGS
    STATIC 
    RESOURCES ${IMG_FILES}
    IMPORTS
      TARGET Gentau::Qml::Foundation
)