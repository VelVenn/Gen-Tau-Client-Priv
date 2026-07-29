configure_file(
    version.hpp.in
    "${CMAKE_CURRENT_BINARY_DIR}/inc/conf/version.hpp"
    @ONLY
    NEWLINE_STYLE UNIX
)

gt_register_mod(
    NAME conf
    TYPE INTERFACE 
    GEN_ANCHOR
)