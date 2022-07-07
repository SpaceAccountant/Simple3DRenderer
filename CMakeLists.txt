cmake_minimum_required( VERSION 3.21 )

project( Simple3D
	VERSION 0.1.0
	DESCRIPTION "A 3D engine for terminals."
	#HOMEPAGE_URL "https://www.spaceaccountant.me/projects/S3D"
	LANGUAGES C
)

set( CMAKE_C_STANDARD_REQUIRED True )

if ( NOT CMAKE_SYSTEM_PROCESSOR )
	message( FATAL_ERROR "CMAKE_SYSTEM_PROCESSOR not specified" )
endif()

string( TOLOWER ${CMAKE_BUILD_TYPE} PROJECT_BUILD_TYPE )
set( PROJECT_ARCH ${CMAKE_SYSTEM_PROCESSOR} )
set( PROJECT_PLATFORM ${CMAKE_SYSTEM_NAME} )

# Redirect target artifacts to more specific directories.
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_ARCH}/${PROJECT_PLATFORM}/lib )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_ARCH}/${PROJECT_PLATFORM}/lib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_ARCH}/${PROJECT_PLATFORM}/bin )

# Configure the project math library.
add_subdirectory( src/math )
set( PROJECT_MATH_LIBRARY ${IMPORT_MATH_LIBRARY} )
message( "Using math library \"${PROJECT_MATH_LIBRARY}\"." )

# Configure the project rendering libraries.
add_subdirectory( src/graphics )
set( PROJECT_RENDER_LIBRARIES ${IMPORT_RENDER_LIBRARIES} )
message( "Using rendering libraries:" )
foreach( RENDER_LIBRARY IN LISTS PROJECT_RENDER_LIBRARIES )
	message( "  - \"${RENDER_LIBRARY}\"" )
endforeach()

# Configure the project core application.
add_subdirectory( src/core )
