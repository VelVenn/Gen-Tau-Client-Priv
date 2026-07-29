include_guard(GLOBAL)

# =========================== GEN_TAU LOGGING OPTIONS ============================
option(GEN_TAU_LOG_ENABLED "Enable Gen-τ logging" ON)

option(GEN_TAU_LOG_TO_CONSOLE "Enable Gen-τ logging to console" OFF)

option(GEN_TAU_LOG_TO_FILE "Enable Gen-τ logging to file" ON)

set(GEN_TAU_LOG_LEVEL "DEFAULT" CACHE STRING "The minimum log level to compile in")
set_property(
  CACHE GEN_TAU_LOG_LEVEL 
  PROPERTY STRINGS 
  "TRACE" 
  "DEBUG" 
  "INFO" 
  "WARN" 
  "ERROR" 
  "CRITICAL" 
  "DEFAULT"
)
# =========================== GEN_TAU LOGGING OPTIONS ============================

# ======================= GEN_TAU UDP HEVC STREAM OPTIONS ========================
option(GEN_TAU_VT_HEADER_FROM_BI "Use BI header parsing for Gen-τ UDP HEVC stream" ON)
# ======================= GEN_TAU UDP HEVC STREAM OPTIONS ========================

# ====================== GEN_TAU WAYLAND SUPPORT OPTIONS =========================
set(GEN_TAU_WAYLAND "AUTO" CACHE STRING
  "Enable Gen-τ Wayland support. AUTO will enable if Wayland is detected, ON will force enable, OFF will force disable"
)
set_property(CACHE GEN_TAU_WAYLAND PROPERTY STRINGS AUTO ON OFF)

string(TOUPPER "${GEN_TAU_WAYLAND}" GEN_TAU_WAYLAND)

if(NOT GEN_TAU_WAYLAND MATCHES "^(AUTO|ON|OFF)$")
  message(FATAL_ERROR
    "GEN_TAU_WAYLAND must be AUTO, ON, or OFF"
  )
endif()
# ====================== GEN_TAU WAYLAND SUPPORT OPTIONS =========================

# ========================= GEN_TAU CMAKE DEBUG OPTIONS ==========================
option(GEN_TAU_CMAKE_VERBOSE "Enable Gen-τ CMake verbose output" OFF)
# ========================= GEN_TAU CMAKE DEBUG OPTIONS ==========================

# ========================= GEN_TAU GLOBAL TEST OPTIONS ==========================
option(GEN_TAU_BUILD_TESTS "Build Gen-τ tests" OFF)
# ========================= GEN_TAU GLOBAL TEST OPTIONS ==========================