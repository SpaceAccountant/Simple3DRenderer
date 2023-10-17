# Attempt to find the name of the system curses library if not specified.
if( NOT PROJECT_CURSES_LIBRARY )
	find_library( PROJECT_CURSES_LIBRARY
	NAMES
		ncurses
		pdcurses
	HINTS
		${PROJECT_SOURCE_DIR}/lib/${PROJECT_BUILD_TYPE}/${PROJECT_ARCH}/${PROJECT_PLATFORM}
	NO_CACHE
	REQUIRED
	)
endif()

set_target_properties( ${RENDER_LIBRARY}
PROPERTIES
	C_STANDARD 17
)
target_sources( ${RENDER_LIBRARY}
PRIVATE
	curses/CursesDevice.c
)
target_link_libraries( ${RENDER_LIBRARY}
PUBLIC
    ${PROJECT_CURSES_LIBRARY}
)
