cmake_minimum_required( VERSION 3.21 )

set( EXPORT_MATH_LIBRARY math )

# Configure the math library to export to the project.
message( "Configuring math library \"${EXPORT_MATH_LIBRARY}\"..." )
add_library( ${EXPORT_MATH_LIBRARY} STATIC )
set_target_properties( ${EXPORT_MATH_LIBRARY} PROPERTIES
	C_STANDARD 17
)
target_sources( ${EXPORT_MATH_LIBRARY}
PRIVATE
	VecMath.c
	VecTypes.c
)
target_include_directories( ${EXPORT_MATH_LIBRARY}
PUBLIC
	${PROJECT_SOURCE_DIR}/src
)
target_link_libraries( ${EXPORT_MATH_LIBRARY}
PUBLIC
	m
)
message( "Configured math library." )

# Export the math library to the project lists file.
set( IMPORT_MATH_LIBRARY ${EXPORT_MATH_LIBRARY} PARENT_SCOPE )
