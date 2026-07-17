set_source_files_properties(
  UiPref.qml
  PROPERTIES QT_QML_SINGLETON_TYPE TRUE
)

gt_register_qml_mod(
  TARGET settings-ui-pref
  URI_SUFFIX Settings.UiPref
  REAL_TARGET_VAR SET_UI_PREF_TAR
  QML_FILES
    UiPref.qml
  QT_ARGS
    STATIC
)

target_link_libraries(${SET_UI_PREF_TAR}
  PRIVATE
    Qt6::Quick
    Qt6::Core
)