gt_register_protobuf_mod(
  TARGET msg
  QML
  URI_SUFFIX Message
  PROTO_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/message.proto
  QT_ARGS
    HEADER_GUARD pragma
)
