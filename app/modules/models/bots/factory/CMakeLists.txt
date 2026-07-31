file(
  GLOB_RECURSE SRC_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "inc/*.hpp" "*.cpp"
)

gt_register_qt_lib(
  TARGET bots-factory
  REAL_TARGET_VAR BOTS_FACTORY_TAR
  QT_ARGS
    STATIC
    ${SRC_FILES}
)

target_include_directories(${BOTS_FACTORY_TAR}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)

target_link_libraries(${BOTS_FACTORY_TAR}
  PUBLIC
    Qt6::Core 

  PRIVATE 
    Qt6::Quick
    Gentau::Utils
    Gentau::App::BotsCommon
    Gentau::App::BotsHero
    Gentau::App::BotsInfantry
)
