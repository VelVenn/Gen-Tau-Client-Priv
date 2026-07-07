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

# ========================= GEN_TAU CMAKE DEBUG OPTIONS ==========================
option(GEN_TAU_CMAKE_VERBOSE "Enable Gen-τ CMake verbose output" OFF)
# ========================= GEN_TAU CMAKE DEBUG OPTIONS ==========================

# ========================= GEN_TAU GLOBAL TEST OPTIONS ==========================
option(GEN_TAU_BUILD_TESTS "Build Gen-τ tests" OFF)
# ========================= GEN_TAU GLOBAL TEST OPTIONS ==========================