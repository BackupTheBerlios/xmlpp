# Find dependencies
FIND_PACKAGE ( Boost 1.36.0 	REQUIRED	COMPONENTS signals thread system filesystem)
FIND_PACKAGE ( XML++            REQUIRED )
FIND_PACKAGE ( SimpleGL         REQUIRED )
FIND_PACKAGE ( SDL       		REQUIRED )
FIND_PACKAGE ( Bullet )

# Find SlonEngine
SET (WANTED_MODULE_NAME     SlonEngine)
SET (WANTED_INCLUDES        slon/Engine.h)
SET (WANTED_LIBRARIES       SlonEngine)
SET (WANTED_DEPENDENCIES    Boost XML++ SimpleGL SDL)

# optional dependencies
IF (BULLET_FOUND)
    LIST ( APPEND WANTED_DEPENDENCIES Bullet )
ENDIF (BULLET_FOUND)

INCLUDE (FindCommonModule)

