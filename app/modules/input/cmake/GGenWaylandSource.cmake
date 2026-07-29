include_guard(GLOBAL)

if(GEN_TAU_WAYLAND_AVAILABLE)
  qt_generate_wayland_protocol_client_sources(${INPUT_TAR}
    PRIVATE_CODE
    FILES
      "${POINTER_CONSTRAINTS_PROTOCOL}"
      "${RELATIVE_POINTER_PROTOCOL}"
  )
endif()