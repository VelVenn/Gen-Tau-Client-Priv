# =========================== ENABLE SANITIZER ===========================
option(GEN_TAU_USE_ASAN "Enable address sanitizer for all gen-τ targets" OFF)
option(GEN_TAU_USE_TYPE_SAN "Enable type sanitizer for all gen-τ targets" OFF)
option(GEN_TAU_SAN_OPT_LEVEL "Optimization level for address sanitizer builds (default: O0)" O0)

if(GEN_TAU_USE_ASAN AND GEN_TAU_USE_TYPE_SAN)
  message(FATAL_ERROR "!! Cannot enable both Address Sanitizer and Type Sanitizer at the same time !!")
endif()

if(GEN_TAU_USE_ASAN OR GEN_TAU_USE_TYPE_SAN)
  set(CMAKE_C_COMPILER clang CACHE STRING "" FORCE)
  set(CMAKE_CXX_COMPILER clang++ CACHE STRING "" FORCE)

  set(LLVM_ENABLE_PROJECTS "clang" CACHE STRING "" FORCE)
  set(LLVM_ENABLE_RUNTIMES "compiler-rt" CACHE STRING "" FORCE)

  message(STATUS "-> Sanitizer enabled, forcing Clang as the compiler, optimazation level: ${GEN_TAU_SAN_OPT_LEVEL}")

  add_compile_options(
    -fsanitize-ignorelist=${CMAKE_SOURCE_DIR}/scripts/.san-ignore
    -${GEN_TAU_SAN_OPT_LEVEL}
    -g
  )
endif()

if(GEN_TAU_USE_ASAN)
  message(STATUS "-> Address Sanitizer enabled")  
  add_compile_options(
    -fsanitize=address,undefined
    -fno-omit-frame-pointer
    -fno-optimize-sibling-calls
    -fsanitize-address-use-after-scope
    -fsanitize-recover=address
  )
  add_link_options(-fsanitize=address,undefined)

  if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING "! You are using Address Sanitizer in non-Debug mode, which may lead to unexpected behavior and performance issues. !")
  endif()
endif()

if(GEN_TAU_USE_TYPE_SAN)
  message(STATUS "-> Type Sanitizer enabled")
  message(STATUS "-> Please note that if '-fno-strict-aliasing' is set, Type Sanitizer will not work")
  
  set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
  message(STATUS "-> Forcing build type to Release for Type Sanitizer")

  add_compile_options(
    -fsanitize=type
    -fno-omit-frame-pointer
    -fno-optimize-sibling-calls
  )
  add_link_options(-fsanitize=type)
endif()
# =========================== ENABLE SANITIZER ===========================