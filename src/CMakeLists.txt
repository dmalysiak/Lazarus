#global cmake configuration file
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/CMakeConfig.h.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/CMakeConfig.h"
)

# Find all the input files
FILE(GLOB_RECURSE inFiles RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")


add_library(lazarus_shared_debug SHARED ${inFiles})
target_link_libraries(lazarus_shared_debug ${LMSENSORS_LIB} ${BLOCKID_LIB} ${ZLIB_LIB} crypt rt dl gomp m pthread ${TOMCRYPT_LIB} ${CPUPOWER_LIB} )
set_property(TARGET lazarus_shared_debug PROPERTY COMPILE_FLAGS "${GENERAL_BUILD_OPT} ${DEBUG_BUILD_OPT}")
set_property(TARGET lazarus_shared_debug PROPERTY LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/shared/debug)

add_library(lazarus_shared_release SHARED ${inFiles})
target_link_libraries(lazarus_shared_release ${LMSENSORS_LIB} ${BLOCKID_LIB} ${ZLIB_LIB} crypt rt dl gomp m pthread ${TOMCRYPT_LIB} ${CPUPOWER_LIB})
set_property(TARGET lazarus_shared_release PROPERTY COMPILE_FLAGS "${GENERAL_BUILD_OPT} ${RELEASE_BUILD_OPTIMIZATION_OPT}")
set_property(TARGET lazarus_shared_release PROPERTY LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/shared/release)

add_library(lazarus_static_debug STATIC ${inFiles})
target_link_libraries(lazarus_static_debug ${LMSENSORS_LIB} ${BLOCKID_LIB} ${ZLIB_LIB} crypt rt dl gomp m pthread ${TOMCRYPT_LIB} ${CPUPOWER_LIB})
set_property(TARGET lazarus_static_debug PROPERTY COMPILE_FLAGS "${GENERAL_BUILD_OPT} ${DEBUG_BUILD_OPT}")
set_property(TARGET lazarus_static_debug PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/static/debug)

add_library(lazarus_static_release STATIC ${inFiles})
target_link_libraries(lazarus_static_release ${LMSENSORS_LIB} ${BLOCKID_LIB} ${ZLIB_LIB} crypt rt dl gomp m pthread ${TOMCRYPT_LIB} ${CPUPOWER_LIB})
set_property(TARGET lazarus_static_release PROPERTY COMPILE_FLAGS "${GENERAL_BUILD_OPT} ${RELEASE_BUILD_OPTIMIZATION_OPT}")
set_property(TARGET lazarus_static_release PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/static/release)
