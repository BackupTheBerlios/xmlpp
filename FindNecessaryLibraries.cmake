# boost
INCLUDE (FindBoost)
SET (Boost_ADDITIONAL_VERSIONS 1.3*.*)
FIND_PACKAGE (Boost 1.36.0)
IF (NOT Boost_FOUND)
	MESSAGE (FATAL_ERROR "Project ${CMAKE_PROJECT_NAME} couldn't be completely build whithout boost libraries")
ENDIF (NOT Boost_FOUND)
