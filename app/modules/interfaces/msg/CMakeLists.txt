gt_register_qml_mod(
  TARGET msg
  URI_SUFFIX Message
  REAL_TARGET_VAR MSG_TAR
  QT_ARGS
    STATIC
)

gt_register_protobuf_mod(
  TARGET msg
  NO_ALIAS
  PROTO_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/message.proto
  QT_ARGS
    HEADER_GUARD pragma
)

target_link_libraries(${MSG_TAR}
  PUBLIC
    Qt6::Qml
    Qt6::Protobuf
)