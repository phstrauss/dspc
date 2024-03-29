cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(dspc C CXX)

set(DSPC_VERSION 1.1.2)

set(INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Installation directory for executables")
set(INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "Installation directory for libraries")
set(INSTALL_INC_DIR "${CMAKE_INSTALL_PREFIX}/include/dspc" CACHE PATH "Installation directory for headers")
set(INSTALL_MAN_DIR "${CMAKE_INSTALL_PREFIX}/share/man" CACHE PATH "Installation directory for manual pages")
set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" CACHE PATH "Installation directory for pkgconfig (.pc) files")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

ADD_DEFINITIONS("-D__GNUC_STDC_INLINE__")

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    ADD_DEFINITIONS("-D__DARWIN__")
    set(CMAKE_C_COMPILER "clang")
    set(CMAKE_CXX_COMPILER "clang++")
endif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_C_COMPILER "gcc")
    set(CMAKE_CXX_COMPILER "g++")
endif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

set(CC ${CMAKE_C_COMPILER})
set(CXX ${CMAKE_CXX_COMPILER})

set(CMAKE_C_FLAGS_DEBUG "-DDEBUG -Wall -std=c99 -g -fsanitize=address")

set(CMAKE_C_FLAGS_RELEASE "-D_FORTIFY_SOURCE_2 -Wall -std=c99 -O3 -m64 -mavx2 -fstack-protector-strong -funroll-loops -floop-block -freschedule-modulo-scheduled-loops -fselective-scheduling2 -fsel-sched-pipelining -fsel-sched-pipelining-outer-loops -ftree-vectorize -fpredictive-commoning -fprefetch-loop-arrays -floop-nest-optimize -ftree-loop-if-convert -ftree-loop-if-convert-stores")

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -Wall -std=c++11 -g -fsanitize=address")

set(CMAKE_CXX_FLAGS_RELEASE "-D_FORTIFY_SOURCE_2 -Wall -std=c++11 -O3 -m64 -mavx2 -fstack-protector-strong -funroll-loops -floop-block -freschedule-modulo-scheduled-loops -fselective-scheduling2 -fsel-sched-pipelining -fsel-sched-pipelining-outer-loops -ftree-vectorize -fpredictive-commoning -fprefetch-loop-arrays -floop-nest-optimize -ftree-loop-if-convert -ftree-loop-if-convert-stores")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS_RELEASE})
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_RELEASE})
else()
    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS_DEBUG})
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_DEBUG})
endif()

include(FindPkgConfig)
pkg_check_modules(FFTW REQUIRED fftw3f)
pkg_check_modules(SNDFILE REQUIRED sndfile)

include_directories(${FFTW_INCLUDE_DIRS} ${SNDFILE_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} "/usr/local/include")

set(DSPC_PUBLIC_HEADERS
    # were in genc
    dspc/crc32_hswarren.h
    dspc/dumb_alloc2d.h
    dspc/perr.h
    dspc/strip_path.h
    dspc/cycle-fftw.h
    dspc/gnuinline.h
    dspc/dspc.h
    dspc/exception.hh
    dspc/mem.hh
    dspc/templating.hh
    # dspc
    dspc/dspc_misc.h
    dspc/fft.hh
    dspc/ft_utils.h
    dspc/rng.h
    dspc/sfio.hh
    dspc/windows.hh
    dspc/butterworth_lr.h
    dspc/funcname.h
    dspc/genfiltering_c_preproc.h
    dspc/iir_c_preproc.h
    dspc/iir_utils.h
    dspc/path_biquad_cpp.hh
    dspc/path_filtering_c.h
    dspc/path_iir_cpp.hh
    dspc/path_vec16_cintr.h
    dspc/jclient.h
    # solvers
    dspc/de.h
    dspc/remez.h
)

set(DSPC_PRIVATE_HEADERS
    dspc/de_rng_helpers.h
    dspc/de_strategy_helpers.h
)

set(DSPC_SRCS
    # were in genc
    crc32_hswarren.c
    dumb_alloc2d.c
    perr.c
    strip_path.c
    exception.cpp
    mem.cpp
    # dspc
    ft_utils.c
    rng.c
    sfio.cpp
    windows.cpp
    windows_flac.c
    iir_utils.c
    path_biquad_cpp.cpp
    jclient.c
    # solvers
    de_strategy.c
    de_solve.c
    remez.c
)

set_source_files_properties(cpuinfo.c PROPERTIES COMPILE_FLAGS "-O1")

add_library(dspc SHARED ${DSPC_SRCS})

add_library(dspcstatic STATIC ${DSPC_SRCS})

# interesting stuff for the mac : MACOSX_BUNDLE Properties

set_target_properties(dspc PROPERTIES OUTPUT_NAME dspc VERSION ${DSPC_VERSION} SOVERSION 1 INSTALL_NAME_DIR ${INSTALL_LIB_DIR})

set_target_properties(dspc dspcstatic PROPERTIES OUTPUT_NAME dspc)

target_link_libraries(dspc "-L/usr/local/lib/" ${FFTW_LDFLAGS} ${SNDFILE_LDFLAGS})

install(TARGETS dspc dspcstatic
    RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
    LIBRARY DESTINATION "${INSTALL_LIB_DIR}")

install(FILES ${DSPC_PUBLIC_HEADERS} DESTINATION "${INSTALL_INC_DIR}")

## add_executable(t_wavgen1 t_wavgen1.cpp spectro_xy_glv.cpp)
## target_link_libraries(t_wavgen1 dspc "-L/opt/av/lib -lGLV -framework OpenGL -framework GLUT" )
## set_target_properties(t_wavgen1 PROPERTIES COMPILE_FLAGS "-I/opt/av/include -I${DSPC_SOURCE_DIR}")
## add_test(t_wavgen1 t_wavgen1)
