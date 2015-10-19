# Author: Jérémy Levallois <jeremy.levallois@liris.cnrs.fr>

PROJECT(ICTVTools)

cmake_minimum_required (VERSION 2.6)

FIND_PACKAGE(DGtal 0.9 REQUIRED)
INCLUDE_DIRECTORIES(${DGTAL_INCLUDE_DIRS})
LINK_DIRECTORIES(${DGTAL_LIBRARY_DIRS})

# -----------------------------------------------------------------------------
# Define variables and cmake parameters, and display some information
# -----------------------------------------------------------------------------
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
SET(ICTVTools_VERSION_MAJOR 0)
SET(ICTVTools_VERSION_MINOR 1)
SET(ICTVTools_VERSION_PATCH beta)
SET(ICTVTools_VERSION "${ICTVTools_VERSION_MAJOR}.${ICTVTools_VERSION_MINOR}.${ICTVTools_VERSION_PATCH}")
SET(PROJECT_VERSION "${ICTVTools_VERSION_MAJOR}.${ICTVTools_VERSION_MINOR}.${ICTVTools_VERSION_PATCH}")
SET(VERSION ${ICTVTools_VERSION_MAJOR}.${ICTVTools_VERSION_MINOR}.${ICTVTools_VERSION_PATCH})

# -----------------------------------------------------------------------------
# Check Optional Dependencies
# -----------------------------------------------------------------------------
INCLUDE(CheckICTVToolsOptionalDependencies)

SET(Boost_USE_STATIC_LIBS   ON)
SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_RUNTIME OFF)
SET(Boost_FOUND FALSE)
FIND_PACKAGE(Boost 1.46.0 REQUIRED COMPONENTS program_options)
if ( Boost_FOUND )
  message(STATUS "Boost and boost_program_options found.")
  include_directories( ${Boost_INCLUDE_DIRS} )
  SET(ICTVToolsLibDependencies ${ICTVToolsLibDependencies}
     ${Boost_LIBRAIRIES}
     ${Boost_PROGRAM_OPTIONS_LIBRARY})
   SET(DGtalLibInc ${Boost_INCLUDE_DIRS})
endif( Boost_FOUND )

SET(ICTVTools_SRC
  comparison
  joinFiles
  visuPoints
)

FOREACH(FILE ${ICTVTools_SRC})
  add_executable(${FILE} ${FILE})
  target_link_libraries ( ${FILE} ${DGTAL_LIBRARIES} ${ICTVToolsLibDependencies})
  install(TARGETS ${FILE}
      RUNTIME DESTINATION bin
      LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)
