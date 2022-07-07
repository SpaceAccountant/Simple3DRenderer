cmake_minimum_required( VERSION 3.21 )

set( CORE_APPLICATION s3d )

# Configure the core application.
message( "Configuring application ${CORE_APPLICATION}..." )
add_executable( ${CORE_APPLICATION}
	C3D.c
	GameLoop.c
	ObjLoader.c
	Splash.c
)
set_target_properties( ${CORE_APPLICATION} PROPERTIES
	C_STANDARD 17
)
target_include_directories( ${CORE_APPLICATION}
PUBLIC
	${PROJECT_SOURCE_DIR}/src
)
target_link_libraries( ${CORE_APPLICATION}
PRIVATE
	${PROJECT_RENDER_LIBRARIES}
	${PROJECT_MATH_LIBRARY}
)
message( "Configured application." )
