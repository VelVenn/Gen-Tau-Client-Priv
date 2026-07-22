include_guard(GLOBAL)

# ============================================================================
# Internal Qt module naming helpers
# ============================================================================
# 根据模块短标识统一生成实际构建目标、链接别名与默认 QML URI
function(_gt_qt_resolve_module_identity MODULE_STEM)
  string(REPLACE "-" ";" NAME_PARTS "${MODULE_STEM}")
  set(PASCAL_NAME "")

  foreach(PART IN LISTS NAME_PARTS)
    string(SUBSTRING "${PART}" 0 1 FIRST_CHAR)
    string(TOUPPER "${FIRST_CHAR}" FIRST_CHAR)
    string(SUBSTRING "${PART}" 1 -1 REST_CHARS)
    string(APPEND PASCAL_NAME "${FIRST_CHAR}${REST_CHARS}")
  endforeach()

  set(BACKING_TARGET "${GT_APP_MOD_PREFIX}${MODULE_STEM}" PARENT_SCOPE)
  set(LINK_ALIAS "${GT_EXPORT_APP_MOD_NS}${PASCAL_NAME}" PARENT_SCOPE)
  set(IMPORT_URI "${GT_QML_MOD_URI_PREFIX}.${PASCAL_NAME}" PARENT_SCOPE)
endfunction()

# 创建模块链接别名；同一别名已指向相同目标时直接复用
function(_gt_qt_add_module_alias COMMAND_LABEL MODULE_STEM BACKING_TARGET LINK_ALIAS SUPPRESS_ALIAS)
  set(ALIAS_KIND "Link")
  if(GT_EXPORT_QML_MOD_NS)
    string(FIND "${LINK_ALIAS}" "${GT_EXPORT_QML_MOD_NS}" QML_ALIAS_POSITION)
    if(QML_ALIAS_POSITION EQUAL 0)
      set(ALIAS_KIND "QML")
    endif()
  endif()
  if(ALIAS_KIND STREQUAL "Link" AND GT_EXPORT_APP_MOD_NS)
    string(FIND "${LINK_ALIAS}" "${GT_EXPORT_APP_MOD_NS}" APP_ALIAS_POSITION)
    if(APP_ALIAS_POSITION EQUAL 0)
      set(ALIAS_KIND "App")
    endif()
  endif()

  if(SUPPRESS_ALIAS)
    message(STATUS "${COMMAND_LABEL} -> ${MODULE_STEM}: ${ALIAS_KIND} alias will not be assigned")
    return()
  endif()

  if(TARGET ${LINK_ALIAS})
    get_target_property(EXISTING_DESTINATION ${LINK_ALIAS} ALIASED_TARGET)
    if(EXISTING_DESTINATION STREQUAL BACKING_TARGET)
      message(STATUS "${COMMAND_LABEL} -> ${MODULE_STEM}: ${ALIAS_KIND} alias '${LINK_ALIAS}' already assigned to '${BACKING_TARGET}'")
      return()
    endif()

    message(
      FATAL_ERROR
      "!! ${COMMAND_LABEL} -> ${MODULE_STEM}: ${ALIAS_KIND} alias '${LINK_ALIAS}' conflicts with an existing target !!"
    )
  endif()

  add_library(${LINK_ALIAS} ALIAS ${BACKING_TARGET})
  message(STATUS "${COMMAND_LABEL} -> ${MODULE_STEM}: ${ALIAS_KIND} alias '${LINK_ALIAS}' assigned to '${BACKING_TARGET}'")
endfunction()

# 以 QT_ARGS 为分界，将包装层参数与 Qt 原始透传参数分开
function(_gt_qt_partition_arguments RAW_ARGUMENTS)
  list(FIND RAW_ARGUMENTS "QT_ARGS" DELIMITER_POSITION)
  if(DELIMITER_POSITION EQUAL -1)
    set(WRAPPER_INPUT "${RAW_ARGUMENTS}" PARENT_SCOPE)
    set(QT_PASSTHROUGH "" PARENT_SCOPE)
    return()
  endif()

  if(DELIMITER_POSITION GREATER 0)
    list(SUBLIST RAW_ARGUMENTS 0 ${DELIMITER_POSITION} WRAPPER_INPUT)
  else()
    set(WRAPPER_INPUT "")
  endif()

  math(EXPR PASSTHROUGH_BEGIN "${DELIMITER_POSITION} + 1")
  list(LENGTH RAW_ARGUMENTS ARGUMENT_COUNT)
  if(PASSTHROUGH_BEGIN LESS ARGUMENT_COUNT)
    list(SUBLIST RAW_ARGUMENTS ${PASSTHROUGH_BEGIN} -1 QT_PASSTHROUGH)
  else()
    set(QT_PASSTHROUGH "")
  endif()

  set(WRAPPER_INPUT "${WRAPPER_INPUT}" PARENT_SCOPE)
  set(QT_PASSTHROUGH "${QT_PASSTHROUGH}" PARENT_SCOPE)
endfunction()

# 仅在包装层与 QT_ARGS 同时控制同一语义时拒绝重复参数
function(_gt_qt_reject_duplicate_argument COMMAND_LABEL MODULE_STEM WRAPPER_KEY WRAPPER_VALUE QT_KEY QT_PASSTHROUGH)
  if(NOT WRAPPER_VALUE)
    return()
  endif()

  list(FIND QT_PASSTHROUGH "${QT_KEY}" DUPLICATE_POSITION)
  if(NOT DUPLICATE_POSITION EQUAL -1)
    message(
      FATAL_ERROR
      "!! ${COMMAND_LABEL} -> ${MODULE_STEM}: '${WRAPPER_KEY}' and QT_ARGS '${QT_KEY}' cannot be specified at the same time !!"
    )
  endif()
endfunction()

# 读取 QT_ARGS 中单值参数的值，供日志与结果回传使用
function(_gt_qt_read_passthrough_value QT_PASSTHROUGH QT_KEY)
  list(FIND QT_PASSTHROUGH "${QT_KEY}" KEY_POSITION)
  if(KEY_POSITION EQUAL -1)
    set(PASSTHROUGH_VALUE "" PARENT_SCOPE)
    return()
  endif()

  math(EXPR VALUE_POSITION "${KEY_POSITION} + 1")
  list(LENGTH QT_PASSTHROUGH PASSTHROUGH_COUNT)
  if(VALUE_POSITION LESS PASSTHROUGH_COUNT)
    list(GET QT_PASSTHROUGH ${VALUE_POSITION} PASSTHROUGH_VALUE)
    set(PASSTHROUGH_VALUE "${PASSTHROUGH_VALUE}" PARENT_SCOPE)
  else()
    set(PASSTHROUGH_VALUE "" PARENT_SCOPE)
  endif()
endfunction()

# ============================================================================
# Qt library registration function
# ============================================================================
function(gt_register_qt_lib)
  set(options NO_ALIAS)
  set(oneValueArgs TARGET REAL_TARGET_VAR)
  set(multiValueArgs "")

  # 解析包装层参数并保留 Qt 原始参数
  _gt_qt_partition_arguments("${ARGV}")
  cmake_parse_arguments(GT_QT_LIB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${WRAPPER_INPUT})
  # ===============

  # 检查包装层参数
  if(GT_QT_LIB_KEYWORDS_MISSING_VALUES)
    message(
      FATAL_ERROR
      "!! gt_register_qt_lib -> Missing values for arguments '${GT_QT_LIB_KEYWORDS_MISSING_VALUES}' !!"
    )
  endif()

  if(NOT GT_QT_LIB_TARGET)
    message(FATAL_ERROR "!! gt_register_qt_lib -> Library target not specified !!")
  else()
    message(STATUS "gt_register_qt_lib -> ${GT_QT_LIB_TARGET}: Registering Qt library")
  endif()

  if(GT_QT_LIB_UNPARSED_ARGUMENTS)
    message(
      FATAL_ERROR
      "!! gt_register_qt_lib -> ${GT_QT_LIB_TARGET}: Unknown arguments '${GT_QT_LIB_UNPARSED_ARGUMENTS}', pass Qt arguments after QT_ARGS !!"
    )
  endif()
  # ===============

  # 解析统一目标名并调用 Qt 创建库
  set(MODULE_STEM "${GT_QT_LIB_TARGET}")
  _gt_qt_resolve_module_identity("${MODULE_STEM}")

  qt_add_library(${BACKING_TARGET} ${QT_PASSTHROUGH})
  # ===============

  # 创建或复用链接别名
  _gt_qt_add_module_alias(
    "gt_register_qt_lib"
    "${MODULE_STEM}"
    "${BACKING_TARGET}"
    "${LINK_ALIAS}"
    "${GT_QT_LIB_NO_ALIAS}"
  )
  # ===============

  # 将实际构建目标回传至调用作用域
  if(GT_QT_LIB_REAL_TARGET_VAR)
    set(${GT_QT_LIB_REAL_TARGET_VAR} "${BACKING_TARGET}" PARENT_SCOPE)
    message(STATUS "gt_register_qt_lib -> ${GT_QT_LIB_TARGET}: Real target exported to variable '${GT_QT_LIB_REAL_TARGET_VAR}'")
  endif()
  # ===============

  # 注册完成
  if(GT_QT_LIB_NO_ALIAS)
    message(STATUS "✓ Qt library registered: ${GT_QT_LIB_TARGET} -> ${BACKING_TARGET}")
  else()
    message(STATUS "✓ Qt library registered: ${GT_QT_LIB_TARGET} -> ${BACKING_TARGET} (${LINK_ALIAS})")
  endif()
endfunction()

# ============================================================================
# QML module registration function
# ============================================================================
function(gt_register_qml_mod)
  set(options ROOT NO_ALIAS)
  set(oneValueArgs TARGET URI_SUFFIX REAL_TARGET_VAR OUTPUT_TARGETS)
  set(multiValueArgs QML_FILES)

  # 解析包装层参数并保留 Qt 原始参数
  _gt_qt_partition_arguments("${ARGV}")
  cmake_parse_arguments(GT_QML "${options}" "${oneValueArgs}" "${multiValueArgs}" ${WRAPPER_INPUT})
  # ===============

  # 检查包装层参数
  if(GT_QML_KEYWORDS_MISSING_VALUES)
    message(
      FATAL_ERROR
      "!! gt_register_qml_mod -> Missing values for arguments '${GT_QML_KEYWORDS_MISSING_VALUES}' !!"
    )
  endif()

  if(GT_QML_UNPARSED_ARGUMENTS)
    message(
      FATAL_ERROR
      "!! gt_register_qml_mod -> Unknown arguments '${GT_QML_UNPARSED_ARGUMENTS}', pass Qt arguments after QT_ARGS !!"
    )
  endif()
  # ===============

  # ROOT 模式直接挂载现有应用目标，普通模式使用统一命名规则
  if(GT_QML_ROOT)
    if(NOT GT_QML_TARGET)
      message(FATAL_ERROR "!! gt_register_qml_mod -> Root QML module target not specified !!")
    endif()

    if(NOT TARGET ${GT_QML_TARGET})
      message(FATAL_ERROR "!! gt_register_qml_mod -> ${GT_QML_TARGET}: Root QML module target does not exist !!")
    endif()

    if(GT_QML_URI_SUFFIX)
      message(FATAL_ERROR "!! gt_register_qml_mod -> Root QML module cannot specify URI_SUFFIX !!")
    endif()

    set(MODULE_STEM "${GT_QML_TARGET}")
    set(BACKING_TARGET "${GT_QML_TARGET}")
    set(IMPORT_URI "${GT_QML_MOD_URI_PREFIX}")
    set(GT_QML_NO_ALIAS TRUE)
  else()
    if(NOT GT_QML_TARGET)
      message(FATAL_ERROR "!! gt_register_qml_mod -> Module target not specified !!")
    endif()

    set(MODULE_STEM "${GT_QML_TARGET}")
    _gt_qt_resolve_module_identity("${MODULE_STEM}")

    if(GT_QML_URI_SUFFIX)
      set(IMPORT_URI "${GT_QML_MOD_URI_PREFIX}.${GT_QML_URI_SUFFIX}")
    endif()
  endif()
  # ===============

  # 从 Qt 原始参数开始组装最终调用参数
  set(QT_INVOCATION ${QT_PASSTHROUGH})
  # ===============

  # 检查包装层与 QT_ARGS 对同一 Qt 参数的重复控制
  set(WRAPPER_OWNS_URI "${GT_QML_ROOT}")
  if(GT_QML_URI_SUFFIX)
    set(WRAPPER_OWNS_URI TRUE)
  endif()

  _gt_qt_reject_duplicate_argument(
    "gt_register_qml_mod" "${MODULE_STEM}"
    "ROOT or URI_SUFFIX" "${WRAPPER_OWNS_URI}"
    "URI" "${QT_PASSTHROUGH}"
  )
  _gt_qt_reject_duplicate_argument(
    "gt_register_qml_mod" "${MODULE_STEM}"
    "QML_FILES" "${GT_QML_QML_FILES}"
    "QML_FILES" "${QT_PASSTHROUGH}"
  )
  _gt_qt_reject_duplicate_argument(
    "gt_register_qml_mod" "${MODULE_STEM}"
    "OUTPUT_TARGETS" "${GT_QML_OUTPUT_TARGETS}"
    "OUTPUT_TARGETS" "${QT_PASSTHROUGH}"
  )
  # ===============

  # 设置 QML URI；未由包装层控制时允许 QT_ARGS 覆盖
  list(FIND QT_PASSTHROUGH "URI" URI_POSITION)
  if(URI_POSITION EQUAL -1)
    list(APPEND QT_INVOCATION URI "${IMPORT_URI}")
  else()
    _gt_qt_read_passthrough_value("${QT_PASSTHROUGH}" "URI")
    set(IMPORT_URI "${PASSTHROUGH_VALUE}")
  endif()
  # ===============

  # 设置统一版本号；QT_ARGS 中的 VERSION 优先
  list(FIND QT_PASSTHROUGH "VERSION" VERSION_POSITION)
  if(VERSION_POSITION EQUAL -1)
    set(MODULE_VERSION "${GT_QML_MOD_VERSION}")
    list(APPEND QT_INVOCATION VERSION "${MODULE_VERSION}")
  else()
    _gt_qt_read_passthrough_value("${QT_PASSTHROUGH}" "VERSION")
    set(MODULE_VERSION "${PASSTHROUGH_VALUE}")
  endif()
  # ===============

  # 添加包装层声明的 QML 文件
  list(FIND QT_PASSTHROUGH "QML_FILES" QML_FILES_POSITION)
  if(QML_FILES_POSITION EQUAL -1 AND GT_QML_QML_FILES)
    list(APPEND QT_INVOCATION QML_FILES ${GT_QML_QML_FILES})
  endif()
  # ===============

  # 记录 Qt 生成目标的接收变量
  if(GT_QML_OUTPUT_TARGETS)
    set(TARGET_LIST_DESTINATION "${GT_QML_OUTPUT_TARGETS}")
    list(APPEND QT_INVOCATION OUTPUT_TARGETS "${TARGET_LIST_DESTINATION}")
  else()
    _gt_qt_read_passthrough_value("${QT_PASSTHROUGH}" "OUTPUT_TARGETS")
    set(TARGET_LIST_DESTINATION "${PASSTHROUGH_VALUE}")
  endif()
  # ===============

  # 确保 OUTPUT_DIRECTORY 末尾与 URI 路径结构一致，避免 qmllint 等工具无法发现模块
  list(FIND QT_PASSTHROUGH "OUTPUT_DIRECTORY" OD_POSITION)
  if(NOT GT_QML_ROOT AND OD_POSITION EQUAL -1)
    string(REPLACE "." "/" URI_PATH "${IMPORT_URI}")
    list(APPEND QT_INVOCATION OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${URI_PATH}")
  endif()
  # ===============

  # 创建或扩展 QML 模块
  message(STATUS "gt_register_qml_mod -> ${MODULE_STEM}: Registering QML module '${IMPORT_URI}'")

  qt_add_qml_module(${BACKING_TARGET} ${QT_INVOCATION})
  # ===============

  # 识别静态 QML 模块，其应用链接入口需要同时带入 backing target 和插件
  set(APP_ALIAS_TARGET "${BACKING_TARGET}")
  set(IS_STATIC_QML_MODULE FALSE)
  get_target_property(PLUGIN_TARGET "${BACKING_TARGET}" QT_QML_MODULE_PLUGIN_TARGET)
  if(PLUGIN_TARGET AND TARGET "${PLUGIN_TARGET}")
    get_target_property(PLUGIN_TARGET_TYPE "${PLUGIN_TARGET}" TYPE)
    if(PLUGIN_TARGET_TYPE STREQUAL "STATIC_LIBRARY")
      set(IS_STATIC_QML_MODULE TRUE)
    endif()
  endif()

  # QML 模块别名按 URI 层级生成，并始终指向持有模块元数据的 backing target
  set(QML_ALIAS_PATH "${IMPORT_URI}")
  set(QML_URI_PREFIX "${GT_QML_MOD_URI_PREFIX}.")
  string(FIND "${QML_ALIAS_PATH}" "${QML_URI_PREFIX}" URI_PREFIX_POSITION)
  if(URI_PREFIX_POSITION EQUAL 0)
    string(LENGTH "${QML_URI_PREFIX}" URI_PREFIX_LENGTH)
    string(SUBSTRING "${QML_ALIAS_PATH}" ${URI_PREFIX_LENGTH} -1 QML_ALIAS_PATH)
  endif()
  string(REPLACE "." "::" QML_ALIAS_PATH "${QML_ALIAS_PATH}")
  set(QML_ALIAS "${GT_EXPORT_QML_MOD_NS}${QML_ALIAS_PATH}")
  # ===============

  # 将 Qt 生成的附加目标回传至调用作用域
  if(TARGET_LIST_DESTINATION)
    set(
      ${TARGET_LIST_DESTINATION}
      "${${TARGET_LIST_DESTINATION}}"
      PARENT_SCOPE
    )
  endif()
  # ===============

  # 普通模块创建或复用链接别名，ROOT 模块沿用应用目标
  if(NOT GT_QML_ROOT)
    if(IS_STATIC_QML_MODULE AND NOT GT_QML_NO_ALIAS)
      set(APP_INTERFACE_TARGET "${BACKING_TARGET}-app-interface")
      add_library(${APP_INTERFACE_TARGET} INTERFACE)
      target_link_libraries(${APP_INTERFACE_TARGET} INTERFACE ${BACKING_TARGET} ${PLUGIN_TARGET})
      set(APP_ALIAS_TARGET "${APP_INTERFACE_TARGET}")
    endif()

    _gt_qt_add_module_alias(
      "gt_register_qml_mod"
      "${MODULE_STEM}"
      "${APP_ALIAS_TARGET}"
      "${LINK_ALIAS}"
      "${GT_QML_NO_ALIAS}"
    )
    if(IS_STATIC_QML_MODULE)
      _gt_qt_add_module_alias(
        "gt_register_qml_mod"
        "${MODULE_STEM}"
        "${BACKING_TARGET}"
        "${LINK_ALIAS}::Api"
        "${GT_QML_NO_ALIAS}"
      )
      _gt_qt_add_module_alias(
        "gt_register_qml_mod"
        "${MODULE_STEM}"
        "${PLUGIN_TARGET}"
        "${LINK_ALIAS}::Plugin"
        "${GT_QML_NO_ALIAS}"
      )
    endif()
    _gt_qt_add_module_alias(
      "gt_register_qml_mod"
      "${MODULE_STEM}"
      "${BACKING_TARGET}"
      "${QML_ALIAS}"
      "${GT_QML_NO_ALIAS}"
    )
  endif()
  # ===============

  # 将实际构建目标回传至调用作用域
  if(GT_QML_REAL_TARGET_VAR)
    set(${GT_QML_REAL_TARGET_VAR} "${BACKING_TARGET}" PARENT_SCOPE)
    message(STATUS "gt_register_qml_mod -> ${MODULE_STEM}: Real target exported to variable '${GT_QML_REAL_TARGET_VAR}'")
  endif()
  # ===============

  # 注册完成
  message(STATUS "✓ QML module registered: ${MODULE_STEM} -> ${BACKING_TARGET} [${IMPORT_URI} ${MODULE_VERSION}]")
endfunction()

# ============================================================================
# Qt Protobuf module registration function
# ============================================================================
function(gt_register_protobuf_mod)
  set(options QML NO_ALIAS)
  set(oneValueArgs TARGET URI_SUFFIX REAL_TARGET_VAR OUTPUT_HEADERS OUTPUT_TARGETS)
  set(multiValueArgs PROTO_FILES)

  # 解析包装层参数并保留 Qt 原始参数
  _gt_qt_partition_arguments("${ARGV}")
  cmake_parse_arguments(GT_PROTO_MOD "${options}" "${oneValueArgs}" "${multiValueArgs}" ${WRAPPER_INPUT})
  # ===============

  # 检查包装层参数
  if(GT_PROTO_MOD_KEYWORDS_MISSING_VALUES)
    message(
      FATAL_ERROR
      "!! gt_register_protobuf_mod -> Missing values for arguments '${GT_PROTO_MOD_KEYWORDS_MISSING_VALUES}' !!"
    )
  endif()

  if(NOT GT_PROTO_MOD_TARGET)
    message(FATAL_ERROR "!! gt_register_protobuf_mod -> Module target not specified !!")
  else()
    message(STATUS "gt_register_protobuf_mod -> ${GT_PROTO_MOD_TARGET}: Registering Qt Protobuf module")
  endif()

  if(GT_PROTO_MOD_UNPARSED_ARGUMENTS)
    message(
      FATAL_ERROR
      "!! gt_register_protobuf_mod -> ${GT_PROTO_MOD_TARGET}: Unknown arguments '${GT_PROTO_MOD_UNPARSED_ARGUMENTS}', pass Qt arguments after QT_ARGS !!"
    )
  endif()

  if(NOT GT_PROTO_MOD_PROTO_FILES)
    message(FATAL_ERROR "!! gt_register_protobuf_mod -> ${GT_PROTO_MOD_TARGET}: Proto files not specified !!")
  endif()
  # ===============

  # 解析统一目标名、链接别名与默认 QML URI
  set(MODULE_STEM "${GT_PROTO_MOD_TARGET}")
  _gt_qt_resolve_module_identity("${MODULE_STEM}")
  # ===============

  # 检查包装层与 QT_ARGS 对同一 Qt 参数的重复控制
  _gt_qt_reject_duplicate_argument(
    "gt_register_protobuf_mod" "${MODULE_STEM}"
    "PROTO_FILES" "${GT_PROTO_MOD_PROTO_FILES}"
    "PROTO_FILES" "${QT_PASSTHROUGH}"
  )
  _gt_qt_reject_duplicate_argument(
    "gt_register_protobuf_mod" "${MODULE_STEM}"
    "QML" "${GT_PROTO_MOD_QML}"
    "QML" "${QT_PASSTHROUGH}"
  )
  _gt_qt_reject_duplicate_argument(
    "gt_register_protobuf_mod" "${MODULE_STEM}"
    "URI_SUFFIX" "${GT_PROTO_MOD_URI_SUFFIX}"
    "QML_URI" "${QT_PASSTHROUGH}"
  )
  _gt_qt_reject_duplicate_argument(
    "gt_register_protobuf_mod" "${MODULE_STEM}"
    "OUTPUT_HEADERS" "${GT_PROTO_MOD_OUTPUT_HEADERS}"
    "OUTPUT_HEADERS" "${QT_PASSTHROUGH}"
  )
  _gt_qt_reject_duplicate_argument(
    "gt_register_protobuf_mod" "${MODULE_STEM}"
    "OUTPUT_TARGETS" "${GT_PROTO_MOD_OUTPUT_TARGETS}"
    "OUTPUT_TARGETS" "${QT_PASSTHROUGH}"
  )
  # ===============

  # 从 Qt 原始参数开始组装最终调用参数，并追加协议文件
  set(QT_INVOCATION ${QT_PASSTHROUGH})
  list(APPEND QT_INVOCATION PROTO_FILES ${GT_PROTO_MOD_PROTO_FILES})
  # ===============

  # 仅在显式指定 QML 时启用 QML 类型生成
  list(FIND QT_PASSTHROUGH "QML" QML_POSITION)
  if(GT_PROTO_MOD_QML)
    list(APPEND QT_INVOCATION QML)
  endif()

  set(GENERATE_QML_TYPES "${GT_PROTO_MOD_QML}")
  if(NOT QML_POSITION EQUAL -1)
    set(GENERATE_QML_TYPES TRUE)
  endif()
  # ===============

  # QML 启用后设置模块 URI；URI_SUFFIX 与原始 QML_URI 互斥
  list(FIND QT_PASSTHROUGH "QML_URI" QML_URI_POSITION)
  if(GENERATE_QML_TYPES AND QML_URI_POSITION EQUAL -1)
    if(GT_PROTO_MOD_URI_SUFFIX)
      set(IMPORT_URI "${GT_QML_MOD_URI_PREFIX}.${GT_PROTO_MOD_URI_SUFFIX}")
    endif()

    list(APPEND QT_INVOCATION QML_URI "${IMPORT_URI}")
  elseif(NOT QML_URI_POSITION EQUAL -1)
    _gt_qt_read_passthrough_value("${QT_PASSTHROUGH}" "QML_URI")
    set(IMPORT_URI "${PASSTHROUGH_VALUE}")
  endif()
  # ===============

  if(GENERATE_QML_TYPES)
    message(STATUS "gt_register_protobuf_mod -> ${MODULE_STEM}: QML URI set to '${IMPORT_URI}'")
  endif()

  # 记录生成头文件与附加目标的接收变量
  if(GT_PROTO_MOD_OUTPUT_HEADERS)
    set(HEADER_LIST_DESTINATION "${GT_PROTO_MOD_OUTPUT_HEADERS}")
    list(APPEND QT_INVOCATION OUTPUT_HEADERS "${HEADER_LIST_DESTINATION}")
  else()
    _gt_qt_read_passthrough_value("${QT_PASSTHROUGH}" "OUTPUT_HEADERS")
    set(HEADER_LIST_DESTINATION "${PASSTHROUGH_VALUE}")
  endif()

  if(GT_PROTO_MOD_OUTPUT_TARGETS)
    set(TARGET_LIST_DESTINATION "${GT_PROTO_MOD_OUTPUT_TARGETS}")
    list(APPEND QT_INVOCATION OUTPUT_TARGETS "${TARGET_LIST_DESTINATION}")
  else()
    _gt_qt_read_passthrough_value("${QT_PASSTHROUGH}" "OUTPUT_TARGETS")
    set(TARGET_LIST_DESTINATION "${PASSTHROUGH_VALUE}")
  endif()
  # ===============

  # 创建或扩展 Protobuf 目标
  qt_add_protobuf(${BACKING_TARGET} ${QT_INVOCATION})
  # ===============

  # 将 Qt 生成结果回传至调用作用域
  if(HEADER_LIST_DESTINATION)
    set(
      ${HEADER_LIST_DESTINATION}
      "${${HEADER_LIST_DESTINATION}}"
      PARENT_SCOPE
    )
  endif()

  if(TARGET_LIST_DESTINATION)
    set(
      ${TARGET_LIST_DESTINATION}
      "${${TARGET_LIST_DESTINATION}}"
      PARENT_SCOPE
    )
  endif()
  # ===============

  # 创建或复用链接别名
  _gt_qt_add_module_alias(
    "gt_register_protobuf_mod"
    "${MODULE_STEM}"
    "${BACKING_TARGET}"
    "${LINK_ALIAS}"
    "${GT_PROTO_MOD_NO_ALIAS}"
  )
  # ===============

  # 将实际构建目标回传至调用作用域
  if(GT_PROTO_MOD_REAL_TARGET_VAR)
    set(${GT_PROTO_MOD_REAL_TARGET_VAR} "${BACKING_TARGET}" PARENT_SCOPE)
    message(STATUS "gt_register_protobuf_mod -> ${MODULE_STEM}: Real target exported to variable '${GT_PROTO_MOD_REAL_TARGET_VAR}'")
  endif()
  # ===============

  # 注册完成
  if(GT_PROTO_MOD_NO_ALIAS)
    message(STATUS "✓ Qt Protobuf module registered: ${MODULE_STEM} -> ${BACKING_TARGET}")
  else()
    message(STATUS "✓ Qt Protobuf module registered: ${MODULE_STEM} -> ${BACKING_TARGET} (${LINK_ALIAS})")
  endif()
endfunction()
