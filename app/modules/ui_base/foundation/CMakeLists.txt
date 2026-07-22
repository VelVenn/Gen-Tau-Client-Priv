file(
  GLOB SRC_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "*.qml"
)

file(
  GLOB FONT_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "fonts/*.ttf" "fonts/*.otf"
)

set_source_files_properties(
  ${SRC_FILES} 
  PROPERTIES QT_QML_SINGLETON_TYPE TRUE
)

gt_register_qml_mod(
  TARGET ui-foundation
  URI_SUFFIX Foundation
  REAL_TARGET_VAR UI_FOUNDATION_TAR
  QML_FILES
    ${SRC_FILES}
  QT_ARGS
    STATIC
    RESOURCES ${FONT_FILES}
)

target_link_libraries(${UI_FOUNDATION_TAR}
  PRIVATE
  Qt6::Quick
)