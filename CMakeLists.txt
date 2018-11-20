cmake_minimum_required(VERSION 3.0.0)

option(VERBOSE_COMPILER_OUTPUT "Verbose compiler output" OFF) 

if(VERBOSE_COMPILER_OUTPUT)
set(CMAKE_VERBOSE_MAKEFILE ON)
endif(VERBOSE_COMPILER_OUTPUT)

project(Lazarus)

#version number
set (Lazarus_VERSION_MAJOR 1)
set (Lazarus_VERSION_MINOR 0)


#---------------------------------------- General settings 

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

#general include paths
include_directories("/usr/local/include")
include_directories("/usr/include")
include_directories("/usr/local")
include_directories("/usr/local/include")
include_directories("/usr/include/libxml2")
include_directories("/usr/local/include/tomcrypt")
include_directories(".")
	
#installation targets
set(INSTALL_DIR_HEADER "/usr/local/include/lazarus" CACHE STRING "Installation path for header files. USE A DEDICATED FOLDER IN ORDER TO USE THE UNINSTALL TARGET!")
set(INSTALL_DIR_LIB "/usr/local/lib/lazarus" CACHE STRING "Installation path for library files. USE A DEDICATED FOLDER IN ORDER TO USE THE UNINSTALL TARGET!")

#build options
set(GENERAL_BUILD_OPT "-DMKL_ILP64 -fopenmp -m64 -DTFM_DESC -fopenmp -std=c++17" CACHE STRING "General build options (unwise to change them!)")
set(RELEASE_BUILD_OPTIMIZATION_OPT "-fPIC -O3 -march=native -Wall" CACHE STRING "Optimizations options for the release build")
set(DEBUG_BUILD_OPT "-fPIC -O0 -ggdb -Wall -fstack-check -fstack-protector-all -fno-omit-frame-pointer" CACHE STRING "Options for the debug build")
#-fno-omit-frame-pointer induces more overhead yet it will allow for a more complete call stack.
	
	
#--------------------------- custom installation targets	
add_custom_target(install_shared_debug 
COMMAND -mkdir ${INSTALL_DIR_HEADER} 
COMMAND -mkdir ${INSTALL_DIR_LIB}
COMMAND cp -f "${PROJECT_SOURCE_DIR}/lib/shared/debug/*" ${INSTALL_DIR_LIB}
COMMAND cp -f -r "${PROJECT_SOURCE_DIR}/src/*" ${INSTALL_DIR_HEADER}  
COMMAND find ${INSTALL_DIR_HEADER} -regex '.*.cpp\\|.*Makefile\\|.*.make\\|.*CMakeFiles\\|.*CMakeLists.txt\\|.*CMakeCache.txt' | xargs rm -rf)

add_custom_target(install_shared_release 
COMMAND -mkdir ${INSTALL_DIR_HEADER} 
COMMAND -mkdir ${INSTALL_DIR_LIB}
COMMAND cp -f "${PROJECT_SOURCE_DIR}/lib/shared/release/*" ${INSTALL_DIR_LIB}
COMMAND cp -f -r "${PROJECT_SOURCE_DIR}/src/*" ${INSTALL_DIR_HEADER}  
COMMAND find ${INSTALL_DIR_HEADER} -regex '.*.cpp\\|.*Makefile\\|.*.make\\|.*CMakeFiles\\|.*CMakeLists.txt\\|.*CMakeCache.txt' | xargs rm -rf)

add_custom_target(install_static_debug 
COMMAND -mkdir ${INSTALL_DIR_HEADER} 
COMMAND -mkdir ${INSTALL_DIR_LIB}
COMMAND cp -f "${PROJECT_SOURCE_DIR}/lib/static/debug/*" ${INSTALL_DIR_LIB}
COMMAND cp -f -r "${PROJECT_SOURCE_DIR}/src/*" ${INSTALL_DIR_HEADER}  
COMMAND find ${INSTALL_DIR_HEADER} -regex '.*.cpp\\|.*Makefile\\|.*.make\\|.*CMakeFiles\\|.*CMakeLists.txt\\|.*CMakeCache.txt' | xargs rm -rf)

add_custom_target(install_static_release 
COMMAND -mkdir ${INSTALL_DIR_HEADER} 
COMMAND -mkdir ${INSTALL_DIR_LIB}
COMMAND cp -f "${PROJECT_SOURCE_DIR}/lib/static/release/*" ${INSTALL_DIR_LIB}
COMMAND cp -f -r "${PROJECT_SOURCE_DIR}/src/*" ${INSTALL_DIR_HEADER}  
COMMAND find ${INSTALL_DIR_HEADER} -regex '.*.cpp\\|.*Makefile\\|.*.make\\|.*CMakeFiles\\|.*CMakeLists.txt\\|.*CMakeCache.txt' | xargs rm -rf)

add_custom_target(install_all 
COMMAND -mkdir ${INSTALL_DIR_HEADER} 
COMMAND -mkdir ${INSTALL_DIR_LIB}
COMMAND cp -f "${PROJECT_SOURCE_DIR}/lib/shared/debug/*" ${INSTALL_DIR_LIB}
COMMAND cp -f "${PROJECT_SOURCE_DIR}/lib/shared/release/*" ${INSTALL_DIR_LIB} 
COMMAND cp -f "${PROJECT_SOURCE_DIR}/lib/static/debug/*" ${INSTALL_DIR_LIB} 
COMMAND cp -f "${PROJECT_SOURCE_DIR}/lib/static/release/*" ${INSTALL_DIR_LIB} 
COMMAND cp -f -r "${PROJECT_SOURCE_DIR}/src/*" ${INSTALL_DIR_HEADER}  
COMMAND find ${INSTALL_DIR_HEADER} -regex '.*.cpp\\|.*Makefile\\|.*.make\\|.*CMakeFiles\\|.*CMakeLists.txt\\|.*CMakeCache.txt' | xargs rm -rf)

add_custom_target(clean_cmake 
COMMAND find ${PROJECT_SOURCE_DIR} -regex '.*Makefile\\|.*.make\\|.*CMakeFiles\\|.*CMakeCache.txt' | xargs rm -rf)
	
add_custom_target(uninstall 
COMMAND rmdir -rf ${INSTALL_DIR_HEADER}
COMMAND rmdir -rf ${INSTALL_DIR_LIB})



#---------------------------------------- LMSENSORS support
SET(LMSENSORS_TYPE "USELMSENSORS" CACHE STRING "compile with lmsensors support")
SET_PROPERTY(CACHE LMSENSORS_TYPE PROPERTY STRINGS USELMSENSORS NOLMSENSORS) 

if( LMSENSORS_TYPE STREQUAL "USELMSENSORS")
    set(CMAKE_LMSENSORS_TYPE "USELMSENSORS")
    SET(LMSENSORS_INCLUDE "/usr/include" CACHE STRING "Includes for LMSENSORS")
    SET(LMSENSORS_LIB_PATH "/usr/lib" CACHE STRING "Lib path for LMSENSORS")
    SET(LMSENSORS_LIB sensors CACHE STRING "LMSENSORS library")
        
    #make the entries visible on the standard view
    MARK_AS_ADVANCED (CLEAR LMSENSORS_INCLUDE)
    MARK_AS_ADVANCED (CLEAR LMSENSORS_LIB_PATH)
    MARK_AS_ADVANCED (CLEAR LMSENSORS_LIB)
endif()
if( LMSENSORS_TYPE STREQUAL "NOLMSENSORS")
    set(CMAKE_LMSENSORS_TYPE "NOLMSENSORS")
    SET(LMSENSORS_INCLUDE CACHE STRING "LMSENSORS library includes")
    SET(LMSENSORS_LIB_PATH CACHE STRING "LMSENSORS library path")
    SET(LMSENSORS_LIB CACHE STRING "LMSENSORS library")
    
    #make the entries invisible on the standard view
    MARK_AS_ADVANCED (FORCE LMSENSORS_INCLUDE)
    MARK_AS_ADVANCED (FORCE LMSENSORS_LIB_PATH)
    MARK_AS_ADVANCED (FORCE LMSENSORS_LIB)
endif()

#dynamic change (i.e. multiple clicks on 'configure' required)
IF(NOT(DEFINED LMSENSORS_TYPE_LAST))
     SET(LMSENSORS_TYPE_LAST "NotAnOption" CACHE STRING "last library loaded")
     MARK_AS_ADVANCED (FORCE LMSENSORS_TYPE_LAST)
ENDIF()

IF(NOT(${LMSENSORS_TYPE} STREQUAL ${LMSENSORS_TYPE_LAST}))
     UNSET(LMSENSORS_INCLUDE CACHE)
     UNSET(LMSENSORS_LIB_PATH CACHE)
     UNSET(LMSENSORS_LIB CACHE)
     SET(LMSENSORS_TYPE_LAST ${LMSENSORS_TYPE} CACHE STRING "Updating Library Project Configuration Option" FORCE)
ENDIF()


#---------------------------------------- Tomcrypt support
SET(TOMCRYPT_INCLUDE ${PROJECT_SOURCE_DIR}/src/Core/Encryption/TomCrypt/src/headers ${PROJECT_SOURCE_DIR}/src/Core/Encryption/TomsFastMath/src/headers CACHE STRING "Includes for TomCrypt")
SET(TOMCRYPT_LIB_PATH ${PROJECT_SOURCE_DIR}/src/Core/Encryption/TomCrypt ${PROJECT_SOURCE_DIR}/src/Core/Encryption/TomsFastMath CACHE STRING "Lib path for TomCrypt")
SET(TOMCRYPT_LIB tomcrypt tfm CACHE STRING "TomCrypt library")


#---------------------------------------- CPU power 
SET(CPUPOWER_TYPE "USECPUPOWER" CACHE STRING "compile with CPU power")
SET_PROPERTY(CACHE CPUPOWER_TYPE PROPERTY STRINGS USECPUPOWER NOCPUPOWER) 

if( CPUPOWER_TYPE STREQUAL "USECPUPOWER")
    set(CMAKE_CPUPOWER_TYPE "USECPUPOWER")
    SET(CPUPOWER_INCLUDE "/usr/include" CACHE STRING "Includes for CPUPOWER" )
    SET(CPUPOWER_LIB_PATH "/usr/lib" CACHE STRING "Lib path for CPUPOWER" )
    SET(CPUPOWER_LIB cpupower CACHE STRING "CPUPOWER library" )
    
    #make the entries visible on the standard view
    MARK_AS_ADVANCED (CLEAR CPUPOWER_INCLUDE)
    MARK_AS_ADVANCED (CLEAR CPUPOWER_LIB_PATH)
    MARK_AS_ADVANCED (CLEAR CPUPOWER_LIB)
    
endif()
if( CPUPOWER_TYPE STREQUAL "NOCPUPOWER")
    set(CMAKE_CPUPOWER_TYPE "NOCPUPOWER")
    SET(CPUPOWER_INCLUDE CACHE STRING "Includes for CPUPOWER" )
    SET(CPUPOWER_LIB_PATH  CACHE STRING "Lib path for CPUPOWER" )
    SET(CPUPOWER_LIB  CACHE STRING "CPUPOWER library" )
    
    #make the entries invisible on the standard view
    MARK_AS_ADVANCED (FORCE CPUPOWER_INCLUDE)
    MARK_AS_ADVANCED (FORCE CPUPOWER_LIB_PATH)
    MARK_AS_ADVANCED (FORCE CPUPOWER_LIB)
    
endif()

#dynamic change (i.e. multiple clicks on 'configure' required)
IF(NOT(DEFINED CPUPOWER_TYPE_LAST))
     SET(CPUPOWER_TYPE_LAST "NotAnOption" CACHE STRING "last library loaded")
     MARK_AS_ADVANCED (FORCE CPUPOWER_TYPE_LAST)
ENDIF()

IF(NOT ( CPUPOWER_TYPE STREQUAL ${CPUPOWER_TYPE_LAST}))
     UNSET(CPUPOWER_INCLUDE CACHE)
     UNSET(CPUPOWER_LIB_PATH CACHE)
     UNSET(CPUPOWER_LIB CACHE)
     SET(CPUPOWER_TYPE_LAST ${CPUPOWER_TYPE} CACHE STRING "Updating Library Project Configuration Option" FORCE)
ENDIF()

#---------------------------------------- ZLIB 
option(OPTION_USE_ZLIB "Compile with ZLIB" ON) 

if( OPTION_USE_ZLIB )
    set(CMAKE_ZLIB_TYPE "USEZLIB")
    SET(ZLIB_INCLUDE "/usr/include" CACHE STRING "Includes for ZLIB" )
    SET(ZLIB_LIB_PATH "/usr/lib" CACHE STRING "Lib path for ZLIB" )
    SET(ZLIB_LIB z CACHE STRING "ZLIB library" )
    
    #make the entries visible on the standard view
    MARK_AS_ADVANCED (CLEAR ZLIB_INCLUDE)
    MARK_AS_ADVANCED (CLEAR ZLIB_LIB_PATH)
    MARK_AS_ADVANCED (CLEAR ZLIB_LIB)
else( OPTION_USE_ZLIB )
    set(CMAKE_ZLIB_TYPE "NOZLIB")
    SET(ZLIB_INCLUDE CACHE STRING "Includes for ZLIB" )
    SET(ZLIB_LIB_PATH CACHE STRING "Lib path for ZLIB" )
    SET(ZLIB_LIB CACHE STRING "ZLIB library" )
    
    #make the entries invisible on the standard view
    MARK_AS_ADVANCED (FORCE ZLIB_INCLUDE)
    MARK_AS_ADVANCED (FORCE ZLIB_LIB_PATH)
    MARK_AS_ADVANCED (FORCE ZLIB_LIB)
endif(OPTION_USE_ZLIB)

#dynamic change (i.e. multiple clicks on 'configure' required)
IF(NOT(DEFINED OPTION_USE_ZLIB_LAST))
     SET(OPTION_USE_ZLIB_LAST OFF CACHE BOOL "last conf")
     MARK_AS_ADVANCED (FORCE OPTION_USE_ZLIB_LAST)
ENDIF()

IF(NOT(${OPTION_USE_ZLIB} STREQUAL ${OPTION_USE_ZLIB_LAST}) )
     UNSET(ZLIB_INCLUDE CACHE)
     UNSET(ZLIB_LIB_PATH CACHE)
     UNSET(ZLIB_LIB CACHE)
     SET(OPTION_USE_ZLIB_LAST ${OPTION_USE_ZLIB} CACHE BOOL "Updating Library Project Configuration Option" FORCE)
ENDIF()

#---------------------------------------- CURL 
option(OPTION_USE_CURL "Compile with CURL" ON) 

if( OPTION_USE_CURL )
    set(CMAKE_CURL_TYPE "USECURL")
    SET(CURL_INCLUDE "/usr/include" CACHE STRING "Includes for CURL" )
    SET(CURL_LIB_PATH "/usr/lib" CACHE STRING "Lib path for CURL" )
    SET(CURL_LIB curl CACHE STRING "CURL library" )
    
    #make the entries visible on the standard view
    MARK_AS_ADVANCED (CLEAR CURL_INCLUDE)
    MARK_AS_ADVANCED (CLEAR CURL_LIB_PATH)
    MARK_AS_ADVANCED (CLEAR CURL_LIB)
else( OPTION_USE_CURL )
    set(CMAKE_CURL_TYPE "NOCURL")
    SET(CURL_INCLUDE CACHE STRING "Includes for CURL" )
    SET(CURL_LIB_PATH CACHE STRING "Lib path for CURL" )
    SET(CURL_LIB CACHE STRING "CURL library" )
    
    #make the entries invisible on the standard view
    MARK_AS_ADVANCED (FORCE CURL_INCLUDE)
    MARK_AS_ADVANCED (FORCE CURL_LIB_PATH)
    MARK_AS_ADVANCED (FORCE CURL_LIB)
endif(OPTION_USE_CURL)

#dynamic change (i.e. multiple clicks on 'configure' required)
IF(NOT(DEFINED OPTION_USE_CURL_LAST))
     SET(OPTION_USE_CURL_LAST OFF CACHE BOOL "last conf")
     MARK_AS_ADVANCED (FORCE OPTION_USE_CURL_LAST)
ENDIF()

IF(NOT(${OPTION_USE_CURL} STREQUAL ${OPTION_USE_CURL_LAST}) )
     UNSET(CURL_INCLUDE CACHE)
     UNSET(CURL_LIB_PATH CACHE)
     UNSET(CURL_LIB CACHE)
     SET(OPTION_USE_CURL_LAST ${OPTION_USE_CURL} CACHE BOOL "Updating Library Project Configuration Option" FORCE)
ENDIF()


#---------------------------------------- TomCrypt

include_directories(${TOMCRYPT_INCLUDE})

#---------------------------------------- Libxml

include_directories("/usr/include/libxml2")


#---------------------------------------- BUILD Configs
option(OPTION_BUILD_SHARED_DEBUG "Build shared debug" ON) 
option(OPTION_BUILD_SHARED_RELEASE "Build shared release" ON) 
option(OPTION_BUILD_STATIC_DEBUG "Build static debug" ON) 
option(OPTION_BUILD_STATIC_RELEASE "Build static release" ON) 


#--------------------------- misc adjustments

#these directives are required as the blas library will be used in all modules due to the fact that its position 
#in the core module implies that.
include_directories(${BLAS_INCLUDE})
link_directories(${BLAS_LIB_PATH})

#--------------------------- configure subprojects (i.e. modules)

#project dirs
subdirs(src)

#--------------------------- custom targets

add_custom_target(
    tomcrypt_lib
    COMMAND cd ${PROJECT_SOURCE_DIR}/src/Encryption/TomsFastMath && make clean && sh buildx64.sh 
    COMMAND cd ${PROJECT_SOURCE_DIR}/src/Encryption/TomsFastMath/src/headers && cp -f tfm.h /usr/local/include  
    COMMAND cd ${PROJECT_SOURCE_DIR}/src/Encryption/TomsFastMath && cp -f libtfm.a /usr/local/lib 
    COMMAND cd ${PROJECT_SOURCE_DIR}/src/Encryption/TomCrypt && make clean && sh buildx64.sh
    COMMAND cd ${PROJECT_SOURCE_DIR}/src/Encryption/TomCrypt && cp -f libtomcrypt.a /usr/local/lib
    COMMAND cp -rf ${PROJECT_SOURCE_DIR}/src/Encryption/TomCrypt/src/headers /usr/local/include
    COMMAND mv /usr/local/include/headers /usr/local/include/tomcrypt 
    VERBATIM
)


