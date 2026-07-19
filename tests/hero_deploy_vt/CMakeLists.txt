qt_add_protobuf(
  HdvtProto 
  PROTO_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/hdvt.proto
  OUTPUT_DIRECTORY 
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

gt_register_test(
  NAME hero_deploy_vt_test 
  INC ${CMAKE_CURRENT_SOURCE_DIR}/inc
  SRC 
    main.cpp 
    hero_deploy_vt_recv.cpp
  USE_QT
  QML_URI 
    HDVT
  QML_FILES 
    qml/DeployVt.qml
  DEPS 
    Qt6::Quick
    Qt6::Protobuf
    utils
    comm
    img-trans
    HdvtProto
    Gentau::App::MqttAdapter
)
