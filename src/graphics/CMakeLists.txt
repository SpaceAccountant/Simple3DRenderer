cmake_minimum_required( VERSION 3.21 )

set( BASE_RENDER_LIBRARY render )

# Configure the back-end independent rendering library to export to the project.
message( "Configuring base rendering library \"${BASE_RENDER_LIBRARY}\"..." )
add_library( ${BASE_RENDER_LIBRARY} STATIC )
set_target_properties( ${BASE_RENDER_LIBRARY} PROPERTIES
	C_STANDARD 17
)
target_sources( ${BASE_RENDER_LIBRARY}
PRIVATE
	ColorMap.c
	Controller.c
	Geometry.c
	Rasterizer.c
)
target_include_directories( ${BASE_RENDER_LIBRARY}
PUBLIC
	${PROJECT_SOURCE_DIR}/src
)
target_link_libraries( ${BASE_RENDER_LIBRARY}
PUBLIC
	${PROJECT_MATH_LIBARARY}
	m
)
message( "Configured base rendering library \"${BASE_RENDER_LIBRARY}\"..." )

# Add the back-end independent rendering library to the export list to the project lists file.
set( EXPORT_RENDER_LIBRARIES ${BASE_RENDER_LIBRARY} )

# The list of renderer back-ends implemented for the platform.
if ( NOT PROJECT_RENDER_BACK_ENDS )
	set( PROJECT_RENDER_BACK_ENDS
		curses
	)
endif()

foreach( BACKEND IN LISTS PROJECT_RENDER_BACK_ENDS )
	set( RENDER_LIBRARY render_${BACKEND} )

	message( "Configuring back-end rendering library \"${RENDER_LIBRARY}\"..." )
	# Configure the back-end rendering library.
	add_library( ${RENDER_LIBRARY} STATIC )
	target_include_directories( ${RENDER_LIBRARY}
	PUBLIC
		${PROJECT_SOURCE_DIR}/src
	)
	target_link_libraries( ${RENDER_LIBRARY}
	PUBLIC
		${BASE_RENDER_LIBRARY}
	)
	# Include the rest of the back-end rendering library configuration.
	include( ${CMAKE_CURRENT_SOURCE_DIR}/${BACKEND}/build.cmake )
	message( "Configured back-end rendering library." )

	# Add the back-end rendering library to the export list to the project lists file.
	set( EXPORT_RENDER_LIBRARIES
		${RENDER_LIBRARY}
		${EXPORT_RENDER_LIBRARIES}
	)
endforeach()

# Export the rendering library list to the project lists file.
set( IMPORT_RENDER_LIBRARIES ${EXPORT_RENDER_LIBRARIES} PARENT_SCOPE )
