SET (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

# documentation
#INCLUDE (Doxy)

# standart
INCLUDE (StandartIncludes)

# try to find 3rdParty directory
#INCLUDE (Find3rdParty)

# libraries 
INCLUDE (FindNecessaryLibraries.cmake)

# global dir
LINK_DIRECTORIES ( ${PROJECT_BINARY_DIR}/lib )

INCLUDE_DIRECTORIES ( 
	${PROJECT_SOURCE_DIR}/include 
	${Boost_INCLUDE_DIR}
)