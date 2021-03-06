SET ( LIB3DS_PATH
	${LIB3DS_PATH}
	"$ENV{PROGRAMFILES}/lib3ds"
)

# glew
FIND_PATH ( 
	LIB3DS_INCLUDE_DIR
	"lib3ds.h"
	PATH_SUFFIXES "include"
	PATHS ${LIB3DS_PATH}
)

FIND_LIBRARY ( 
	LIB3DS_LIBRARY_DEBUG
	NAMES "lib3ds-2_0-d.lib"
	PATH_SUFFIXES "lib" "lib64"
	PATHS ${LIB3DS_PATH}
)

FIND_LIBRARY ( 
	LIB3DS_LIBRARY_RELEASE
	NAMES "lib3ds-2_0.lib"
	PATH_SUFFIXES "lib" "lib64"
	PATHS ${LIB3DS_PATH}
)

SET (LIB3DS_LIBRARIES 
	debug ${LIB3DS_LIBRARY_DEBUG} optimized ${LIB3DS_LIBRARY_RELEASE}
)

IF (LIB3DS_INCLUDE_DIR AND (LIB3DS_LIBRARY_RELEASE OR LIB3DS_LIBRARY_DEBUG) )
	SET (LIB3DS_FOUND TRUE)
ELSE (LIB3DS_INCLUDE_DIR AND (LIB3DS_LIBRARY_RELEASE OR LIB3DS_LIBRARY_DEBUG) )
	SET (LIB3DS_FOUND FALSE)
ENDIF (LIB3DS_INCLUDE_DIR AND (LIB3DS_LIBRARY_RELEASE OR LIB3DS_LIBRARY_DEBUG) )

IF (LIB3DS_FOUND AND NOT LIB3DS_FIND_QUIETLY)
	MESSAGE(STATUS "Found lib3ds: ${LIB3DS_LIBRARIES}")
ENDIF (LIB3DS_FOUND AND NOT LIB3DS_FIND_QUIETLY)

