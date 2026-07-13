qt_add_protobuf(
  ${GT_APP_MOD_PREFIX}msg
  QML 
  QML_URI ${GT_QML_MOD_URI_PREFIX}.Message
  PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/message.proto
  HEADER_GUARD pragma
)

add_library(${GT_EXPORT_APP_MOD_NS}Msg ALIAS ${GT_APP_MOD_PREFIX}msg)