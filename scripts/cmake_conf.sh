#! /bin/bash

SRC_DIR=.
BUILD_DIR="build"
GENERATOR="Ninja"
BUILD_TYPE="Debug"

DO_BUILD=0
JOBS=""

DO_CLEAN=0

VERBOSE=0

DO_ASAN=0
DO_TY_SAN=0
SAN_OPT="O0"

DO_TEST=0

LOG=1
LOG_FILE=1
LOG_CONSOLE=1
LOG_LEVEL="DEFAULT"

while [[ "$#" -gt 0 ]]; do
    case $1 in
        -S|--src-dir)   SRC_DIR="$2";    shift ;;
        -B|--build-dir) BUILD_DIR="$2";  shift ;; # 移走 "-B"
        -G|--generator) GENERATOR="$2";  shift ;; # 移走 "-G"
        -t|--type)      BUILD_TYPE="$2"; shift ;;
        -j|--jobs)      JOBS="$2";       shift ;;

        -v|--verbose)          VERBOSE=1     ;;
        -b|--build)            DO_BUILD=1    ;;       
        -C|--clean-rebuild)    DO_CLEAN=1    ;;
        
        -n|--no-log)           LOG=0         ;;
        --no-log-file)         LOG_FILE=0    ;;
        --no-log-cons)         LOG_CONSOLE=0 ;;
        -L|--log-level)        LOG_LEVEL="$2"; shift ;;

        -m|--asan)             DO_ASAN=1     ;;
        --ty-san)              DO_TY_SAN=1   ;;
        --so)                  SAN_OPT="$2"  ; shift ;;
        -T|--build-test)       DO_TEST=1     ;;
        
        -h|--help)      
            echo "Usage: $0 [options]"
            echo ""
            echo "Configuration Options:"
            echo "  -S, --src-dir DIR      Set source directory (default: .)"
            echo "  -B, --build-dir DIR    Set build directory (default: build)"
            echo "  -G, --generator NAME   Set CMake generator (default: Ninja)"
            echo "  -t, --type TYPE        Set build type (default: Debug)"
            echo "  -v, --verbose          Enable Gen-Tau CMake verbose output (default: off)"
            echo ""
            echo "Build Actions:"
            echo "  -b, --build            Run compilation after configuration"
            echo "  -j, --jobs N           Specifies the number of jobs to run simultaneously"
            echo "  -C, --clean-rebuild    Remove build directory before configure"
            echo ""
            echo "Build Options:"
            echo "  -T, --build-test       Build tests (default: off)"
            echo "  -m, --asan             Enable address sanitizer (default: off)"
            echo "  --ty-san               Enable type sanitizer (default: off)"
            echo "  --so                   Optimization level for sanitizer (default: O0)"
            echo "  -n, --no-log           Disable all logging (default: off)"
            echo "  --no-log-file          Disable log file output (default: off)"
            echo "  --no-log-cons          Disable log console output (default: off)"
            echo "  -L, --log-level LEVEL  Set log level [TRACE|DEBUG|INFO|WARN|ERROR|CRITICAL]"
            echo "General:"
            echo "  -h, --help             Show this help message"
            exit 0 
            ;;

        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift # 移走参数的值
done

# Do clean at here
if [[ $DO_CLEAN -eq 1 ]]; then
    if [ -d "$BUILD_DIR" ]; then
        echo "Warning: Cleaning build directory '$BUILD_DIR'..."
        rm -rf "$BUILD_DIR"
    fi
fi

echo "========================================"
echo "Source Dir : $SRC_DIR"
echo "Build Dir  : $BUILD_DIR"
echo "Generator  : $GENERATOR"
echo "Build Type : $BUILD_TYPE"
if [[ -n "$JOBS" ]]; then
    echo "Build Jobs : $JOBS"
fi
echo "========================================"

cmake -S "$SRC_DIR" -G "$GENERATOR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -B "$BUILD_DIR" \
    -DGEN_TAU_CMAKE_VERBOSE="$VERBOSE" \
    -DGEN_TAU_LOG_ENABLED="$LOG" \
    -DGEN_TAU_LOG_TO_CONSOLE="$LOG_CONSOLE" \
    -DGEN_TAU_LOG_TO_FILE="$LOG_FILE" \
    -DGEN_TAU_LOG_LEVEL="$LOG_LEVEL" \
    -DGEN_TAU_USE_ASAN="$DO_ASAN" \
    -DGEN_TAU_USE_TYPE_SAN="$DO_TY_SAN" \
    -DGEN_TAU_SAN_OPT_LEVEL="$SAN_OPT" \
    -DGEN_TAU_BUILD_TESTS="$DO_TEST" \

if [ $? -ne 0 ]; then
    echo "Error: CMake Configuration failed."
    exit 1
fi

if [[ $DO_BUILD -eq 1 ]]; then
    echo "========================================"
    echo ">>> Starting Build..."
    echo "========================================"
    
    if [[ -n "$JOBS" ]]; then
        cmake --build "$BUILD_DIR" -j "$JOBS"
    else
        cmake --build "$BUILD_DIR"
    fi
fi