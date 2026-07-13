gt_register_qml_mod(
  TARGET msg
  URI_SUFFIX Message
  QT_ARGS
    STATIC
)

gt_register_protobuf_mod(
  TARGET msg
  QML
  NO_ALIAS
  PROTO_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/message.proto
  QT_ARGS
    HEADER_GUARD pragma
)