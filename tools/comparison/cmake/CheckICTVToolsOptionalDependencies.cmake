# -----------------------------------------------------------------------------
# Check Optional Dependencies
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Global options
# -----------------------------------------------------------------------------
message(STATUS "-------------------------------------------------------------------------------")
message(STATUS "DGKhal Library optional configuration:")
message(STATUS "   (to change these values, use ccmake, a graphical")
message(STATUS "   cmake frontend, or define cmake commandline variables")
message(STATUS "   -e.g. '-DWITH_GMP:string=true'-, cf documentation)")
message(STATUS "")

OPTION(WITH_C11 "With C++ compiler C11 features." OFF)
OPTION(WITH_OPENMP "With OpenMP (compiler multithread programming) features." OFF)
OPTION(WITH_GMP "With Gnu Multiprecision Library (GMP)." OFF)
OPTION(WITH_EIGEN "With Eigen3 Linear Algebra Library." OFF)
OPTION(WITH_CGAL "With CGAL." OFF)
OPTION(WITH_QGLVIEWER "With LibQGLViewer for 3D visualization (Qt required)." OFF)
OPTION(WITH_PATATE "With Patate library for geometry OFF (Eigen required)." processing)

#----------------------------------
# Checking clang version on APPLE
#
# When using clang 5.0, DGKhal must
# be compiled with C11 features
#----------------------------------
IF (APPLE)
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    EXECUTE_PROCESS( COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE clang_full_version_string )
    string (REGEX REPLACE ".*LLVM version ([0-9]).*" "\\1" CLANG_VERSION_STRING ${clang_full_version_string})
    if (CLANG_VERSION_STRING VERSION_GREATER 4)
      SET(WITH_C11 ON)
      MESSAGE(STATUS "You are using Clang >= 5.0, I'm forcing the WITH_C11 option")
    endif()
  endif()
endif()
MESSAGE(STATUS " ")
#---------------------------------

IF(WITH_C11)
SET (LIST_OPTION ${LIST_OPTION} [c++11]\ )
message(STATUS "      WITH_C11          true    (C++ compiler C11 features)")
ELSE(WITH_C11)
message(STATUS "      WITH_C11          false   (C++ compiler C11 features)")
ENDIF(WITH_C11)

IF(WITH_OPENMP)
SET (LIST_OPTION ${LIST_OPTION} [OpenMP]\ )
message(STATUS "      WITH_OPENMP       true    (OpenMP multithread features)")
ELSE(WITH_OPENMP)
message(STATUS "      WITH_OPENMP       false   (OpenMP multithread features)")
ENDIF(WITH_OPENMP)

IF(WITH_GMP)
SET (LIST_OPTION ${LIST_OPTION} [GMP]\ )
message(STATUS "      WITH_GMP          true    (Gnu Multiprecision Library)")
ELSE(WITH_GMP)
message(STATUS "      WITH_GMP          false   (Gnu Multiprecision Library)")
ENDIF(WITH_GMP)

IF(WITH_EIGEN)
SET (LIST_OPTION ${LIST_OPTION} [EIGEN]\ )
message(STATUS "      WITH_EIGEN        true    (Eigen3)")
ELSE(WITH_EIGEN)
message(STATUS "      WITH_EIGEN        false   (Eigen3)")
ENDIF(WITH_EIGEN)

IF(WITH_CGAL)
SET (LIST_OPTION ${LIST_OPTION} [CGAL]\ )
message(STATUS "      WITH_CGAL         true    (cgal)")
ELSE(WITH_CGAL)
message(STATUS "      WITH_CGAL         false   (cgal)")
ENDIF(WITH_CGAL)

IF(WITH_PATATE)
SET (LIST_OPTION ${LIST_OPTION} [PATATE]\ )
message(STATUS "      WITH_PATATE       true    (Patate geometry library)")
ELSE(WITH_PATATE)
message(STATUS "      WITH_PATATE       false   (Patate geometry library)")
ENDIF(WITH_PATATE)

If(WITH_QGLVIEWER)
SET (LIST_OPTION ${LIST_OPTION} [QGLVIEWER]\ )
message(STATUS "      WITH_QGLVIEWER    true    (Qt/QGLViewer based 3D Viewer)")
ELSE(WITH_QGLVIEWER)
message(STATUS "      WITH_QGLVIEWER    false   (Qt/QGLViewer based 3D Viewer)")
ENDIF(WITH_QGLVIEWER)

# -----------------------------------------------------------------------------
# Check CPP11
# (They are not compulsory).
# -----------------------------------------------------------------------------
SET(C11_FOUND_DGTAL 0)
SET(C11_AUTO_DGTAL 0)
SET(C11_FORWARD_DGTAL 0)
SET(C11_INITIALIZER_DGTAL 0)
SET(C11_ARRAY 0)
IF(WITH_C11)
  INCLUDE(CheckCPP11)
  IF (CPP11_INITIALIZER_LIST OR CPP11_AUTO OR CP11_FORWARD_LIST)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ")
    SET(C11_FOUND_DGTAL 1)
    IF (CPP11_AUTO)
      SET(C11_AUTO_DGTAL 1)
      SET(C11_FEATURES "${C11_FEATURES} auto")
    ENDIF()
    IF (CPP11_INITIALIZER_LIST)
      SET(C11_INITIALIZER_DGTAL 1)
      SET(C11_FEATURES "${C11_FEATURES} initializer-list")
    ENDIF()
    IF (CPP11_FORWARD_LIST)
      SET(C11_FORWARD_DGTAL 1)
      SET(C11_FEATURES "${C11_FEATURES} std::forward-list")
    ENDIF()
    IF (CPP11_ARRAY)
      SET(C11_ARRAY 1)
      SET(C11_FEATURES "${C11_FEATURES} std::array")
    ENDIF()
    IF (CPP11_RREF_MOVE)
      SET(C11_RREF_MOVE 1)
      SET(C11_FEATURES "${C11_FEATURES} std::move rvalue-reference(&&)")
    ENDIF()
    MESSAGE(STATUS "Supported c++11 features: [${C11_FEATURES} ]")
    ADD_DEFINITIONS("-DWITH_C11 ")
  ELSE()
    MESSAGE(FATAL_ERROR "Your compiler does not support any c++11 feature. Please specify another C++ compiler of disable this WITH_C11 option.")
  ENDIF()
ENDIF(WITH_C11)

# -----------------------------------------------------------------------------
# Look for GMP (The GNU Multiple Precision Arithmetic Library)
# (They are not compulsory).
# -----------------------------------------------------------------------------
SET(GMP_FOUND_DGTAL 0)
IF(WITH_GMP)
  FIND_PACKAGE(GMP REQUIRED)
  IF(GMP_FOUND)
    INCLUDE_DIRECTORIES(${GMP_INCLUDE_DIR})
    SET(GMP_FOUND_DGTAL 1)
    SET(DGtalLibDependencies ${DGtalLibDependencies} ${GMPXX_LIBRARIES} ${GMP_LIBRARIES})
    message(STATUS "GMP and GMPXX found." )
    ADD_DEFINITIONS("-DWITH_GMP ")
    SET(DGtalLibInc ${DGtalLibInc} ${GMP_INCLUDE_DIR})
  ELSE(GMP_FOUND)
    message(FATAL_ERROR "GMP not found. Check the cmake variables associated to this package or disable it." )
  ENDIF(GMP_FOUND)

  try_compile(
    GMP_HAS_IOSTREAM
    ${CMAKE_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/cmake/src/gmp/gmpstream.cpp
    CMAKE_FLAGS
    -DINCLUDE_DIRECTORIES:STRING=${GMP_INCLUDE_DIR}
    -DLINK_LIBRARIES:STRING=${GMPXX_LIBRARIES}\;${GMP_LIBRARIES}
    OUTPUT_VARIABLE OUTPUT
    )

  if ( GMP_HAS_IOSTREAM )
    add_definitions("-DGMP_HAS_IOSTREAM")
    message(STATUS "   * GMPXX has iostream capabilities")
  ELSE(GMP_HAS_IOSTREAM)
    message(STATUS ${OUTPUT})
    message(STATUS "   * GMPXX does not have iostream capabilities")
    message(FATAL_ERROR "GMP has been found but there is a link isuse with some g++ versions. Please check your system or disable the GMP dependency." )
  endif (GMP_HAS_IOSTREAM )
ENDIF(WITH_GMP)

# -----------------------------------------------------------------------------
# Look for Qt (needed by libqglviewer visualization).
# -----------------------------------------------------------------------------
set(QT4_FOUND_DGTAL 0)
IF( WITH_QGLVIEWER)
  find_package(Qt4  COMPONENTS QtCore QtGUI QtXml QtOpenGL REQUIRED)
  if ( QT4_FOUND )
    set(QT4_FOUND_DGTAL 1)
    message(STATUS  "Qt4 found (needed by QGLVIEWER).")
    set(QT_USE_QTXML 1)
    ADD_DEFINITIONS("-DWITH_QT4 ")
    include( ${QT_USE_FILE})
    SET(DGtalLibDependencies ${DGtalLibDependencies} ${QT_LIBRARIES} )
    SET(DGtalLibInc ${DGtalLibInc} ${QT_INCLUDE_DIR})
  else ( QT4_FOUND )
    message(FATAL_ERROR  "Qt4 not found (needed by QGLVIEWER).  Check the cmake variables associated to this package or disable it." )
  endif ( QT4_FOUND )
ENDIF( WITH_QGLVIEWER)

# -----------------------------------------------------------------------------
# Look for QGLViewer for 3D display.
# (They are not compulsory).
# -----------------------------------------------------------------------------
set(QGLVIEWER_FOUND_DGTAL 0)
set( WITH_VISU3D 0)
IF(WITH_QGLVIEWER)
  find_package(QGLVIEWER REQUIRED)
  if(QGLVIEWER_FOUND)

    find_package(OpenGL REQUIRED)
      message(STATUS  "libQGLViewer found.")
    if (OPENGL_GLU_FOUND)
      message(STATUS  "  (OpenGL-GLU ok) " ${OPENGL_INCLUDE_DIR})
    else(OPENGL_GLU_FOUND)
      message(FATAL_ERROR  "  libQGLViewer found but your system does not have OpenGL/GLU modules." )
    endif(OPENGL_GLU_FOUND)

    include_directories( ${QGLVIEWER_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
    set(WITH_VISU3D_QGLVIEWER 1)
    set(QGLVIEWER_FOUND_DGTAL 1)
    ADD_DEFINITIONS("-DWITH_VISU3D_QGLVIEWER ")
    SET(DGtalLibDependencies ${DGtalLibDependencies} ${QGLVIEWER_LIBRARIES} ${OPENGL_LIBRARIES}  )
    set( WITH_VISU3D 1 )
  else ( QGLVIEWER_FOUND )
    message(FATAL_ERROR  "libQGLViewer not found.  Check the cmake variables associated to this package or disable it." )
  endif (QGLVIEWER_FOUND)
ENDIF(WITH_QGLVIEWER)

# -----------------------------------------------------------------------------
# Look for OpenMP
# (They are not compulsory).
# -----------------------------------------------------------------------------
SET(OPENMP_FOUND_DGTAL 0)
IF(WITH_OPENMP)
  FIND_PACKAGE(OpenMP REQUIRED)
  IF(OPENMP_FOUND)
    SET(OPENMP_FOUND_DGTAL 1)
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    SET(CMAKE_C_FLAGS  "${CMAKE_CXX_FLAGS} ${OpenMP_C_FLAGS}")
    ADD_DEFINITIONS("-DWITH_OPENMP ")
    message(STATUS "OpenMP found.")
  ELSE(OPENMP_FOUND)
    message(FATAL_ERROR "OpenMP support not available.")
  ENDIF(OPENMP_FOUND)
ENDIF(WITH_OPENMP)

# -----------------------------------------------------------------------------
# Look for Eigen3
# (They are not compulsory).
# -----------------------------------------------------------------------------
SET(EIGEN_FOUND_DGTAL 0)
IF(WITH_EIGEN)
  FIND_PACKAGE(Eigen3 3.1.1 REQUIRED)
  IF(EIGEN3_FOUND)
    SET(EIGEN_FOUND_DGTAL 1)
    ADD_DEFINITIONS("-DWITH_EIGEN ")
    include_directories( ${EIGEN3_INCLUDE_DIR})
    message(STATUS "Eigen3 (version ${EIGEN3_VERSION}) found.")
  ELSE(EIGEN3_FOUND)
    message(FATAL_ERROR "Eigen3 is not found or the installed version (${EIGEN3_VERSION}) is below 3.1.1. ")
  ENDIF(EIGEN3_FOUND)
ENDIF(WITH_EIGEN)

# -----------------------------------------------------------------------------
# Look for CGAL
# (They are not compulsory).
# -----------------------------------------------------------------------------
SET(CGAL_FOUND_DGTAL 0)
IF(WITH_CGAL)
  IF (WITH_GMP AND  WITH_EIGEN)
    message(STATUS "GMP and Eigen3 detected for CGAL.")
  ELSE()
    message(FATAL_ERROR "CGAL needs GMP and Eigen3. You must activate  WITH_GMP and WITH_EIGEN flags and have the associated package installed.")
  ENDIF()

  find_package(CGAL COMPONENTS Core Eigen3 BLAS LAPACK)
  IF(CGAL_FOUND)
    include( ${CGAL_USE_FILE} )
    SET(CGAL_FOUND_DGTAL 1)
    ADD_DEFINITIONS("-DCGAL_EIGEN3_ENABLED   ")
    ADD_DEFINITIONS("-DWITH_CGAL ")
    SET(DGtalLibDependencies ${DGtalLibDependencies} ${CGAL_LIBRARIES} ${CGAL_3D_PARTY-LIBRARIES} )
    ## Making sure that CGAL got the Eigen3 flag
    ADD_DEFINITIONS("-DWITH_Eigen3 -DWITH_LAPACK ")
    message(STATUS "CGAL found.")
  ENDIF(CGAL_FOUND)
ENDIF(WITH_CGAL)

# -----------------------------------------------------------------------------
# Look for Patate
# http://patate.gforge.inria.fr/html/index.html
# (they Are not compulsory).
# -----------------------------------------------------------------------------
SET(PATATE_FOUND_DGTAL 0)
IF(WITH_PATATE)
  IF (WITH_EIGEN)
    message(STATUS "Eigen3 detected for PATATE.")
  ELSE()
    message(FATAL_ERROR "PATATE needs  Eigen3. You must activate  WITH_EIGEN flags and have the associated package installed.")
  ENDIF()
  find_package(Patate)
  IF(PATATE_FOUND)
    SET(DGtalLibInc ${DGtalLibInc} ${PATATE_INCLUDE_DIR})
    INCLUDE_DIRECTORIES( ${PATATE_INCLUDE_DIR})
    SET(PATATE_FOUND_DGTAL 1)
    ADD_DEFINITIONS("-DWITH_PATATE ")
    ADD_DEFINITIONS("-DWITH_Eigen3 ")
    message(STATUS "PATATE found. ${PATATE_INCLUDE_DIR} ")
 ELSE(PATATE_FOUND)
   message(FATAL_ERROR "Patate headers not found.")
 ENDIF(PATATE_FOUND)
ENDIF(WITH_PATATE)

message(STATUS "-------------------------------------------------------------------------------")