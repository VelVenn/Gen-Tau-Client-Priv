include_guard(GLOBAL)

# =========================== GEN_TAU GENERAL SETTINGS ===========================
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GT_QML_MOD_VERSION_MAJOR 1)
set(GT_QML_MOD_VERSION_MINOR 0)

add_compile_definitions(QT_NO_KEYWORDS) # 避免与其他第三方库API名称冲突
# =========================== GEN_TAU GENERAL SETTINGS ===========================

# ======================= GEN_TAU INTERNAL NAMING OPTIONS ========================
set(GT_APP_NAME "gen-tau")
set(GT_QML_MOD_URI_PREFIX "Gentau")
set(GT_LIB_PREFIX "gt-")
set(GT_EXPORT_LIB_NS "Gentau::")
# ======================= GEN_TAU INTERNAL NAMING OPTIONS ========================

# ========================= GEN_TAU ARTIFACT SETTINGS ============================
set(GT_EXE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(GT_TEST_OUTPUT_PATH "${CMAKE_BINARY_DIR}/tests")
set(GT_LIB_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GT_EXE_OUTPUT_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${GT_LIB_OUTPUT_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${GT_LIB_OUTPUT_PATH})

# 多配置生成器支持
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${CONFIG} CONFIG_UPPER)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${GT_EXE_OUTPUT_PATH})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${GT_LIB_OUTPUT_PATH})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${GT_LIB_OUTPUT_PATH})
endforeach()

# 默认开启位置无关代码，防止动态链接错误
set(CMAKE_POSITION_INDEPENDENT_CODE ON) 
# ========================= GEN_TAU ARTIFACT SETTINGS ============================

# ======================= GEN_TAU GLOBAL DEBUG SETTINGS ==========================
string(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_UPPER)
message(STATUS "Configuring version.hpp with build type: ${CMAKE_BUILD_TYPE_UPPER}")

if(CMAKE_BUILD_TYPE_UPPER STREQUAL "DEBUG")
  message(STATUS "-> Debug mode detected. set GEN_TAU_DEBUG to ON")
  set(GEN_TAU_DEBUG 1)
else()
  message(STATUS "-> Release/Other mode. set GEN_TAU_DEBUG to OFF")
  set(GEN_TAU_DEBUG 0)
endif()
# ======================= GEN_TAU GLOBAL DEBUG SETTINGS ==========================

# ======================== GEN_TAU GLOBAL LOG SETTINGS ===========================
if(GEN_TAU_LOG_LEVEL STREQUAL "DEFAULT")
  message(STATUS "-> Log level set to DEFAULT")
  if(GEN_TAU_DEBUG)
    set(GEN_TAU_ACTIVE_LOG_LEVEL "TRACE")
  else()
    set(GEN_TAU_ACTIVE_LOG_LEVEL "INFO")
  endif()
else()
  message(STATUS "-> Log level set to ${GEN_TAU_LOG_LEVEL}")
  set(GEN_TAU_ACTIVE_LOG_LEVEL "${GEN_TAU_LOG_LEVEL}")
endif()

if(GEN_TAU_LOG_ENABLED)
  if(GEN_TAU_LOG_TO_CONSOLE OR GEN_TAU_LOG_TO_FILE)
    message(STATUS "-> Logging enabled")
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_${GEN_TAU_ACTIVE_LOG_LEVEL})
  else()
    message(WARNING "-> Logging enabled but no log sink selected, disabling it")
    set(GEN_TAU_LOG_ENABLED OFF)
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_OFF)
  endif()
else()
  message(STATUS "-> Logging disabled")
  add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_OFF)
endif()

if(GEN_TAU_LOG_TO_CONSOLE AND GEN_TAU_LOG_ENABLED)
  message(STATUS "-> Log to console enabled")
else()
  message(STATUS "-> Log to console disabled")
endif()

if(GEN_TAU_LOG_TO_FILE AND GEN_TAU_LOG_ENABLED)
  message(STATUS "-> Log to file enabled")
else()
  message(STATUS "-> Log to file disabled")
endif()
# ======================== GEN_TAU GLOBAL LOG SETTINGS ===========================

# ====================== GEN_TAU VT HEADER PARSING MESSAGE =======================
if (GEN_TAU_VT_HEADER_FROM_BI)
  message(STATUS "-> Using BI header parsing for Gen-τ UDP HEVC stream")
else()
  message(STATUS "-> Using LI header parsing for Gen-τ UDP HEVC stream")
endif()
# ====================== GEN_TAU VT HEADER PARSING MESSAGE =======================