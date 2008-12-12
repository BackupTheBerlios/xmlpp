SET (CMAKE_DEBUG_POSTFIX "d")
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# install
SET (INSTALL_INCDIR 	include)
SET (INSTALL_LIBDIR 	lib)
SET (INSTALL_ARCHIVEDIR lib)

IF (WIN32)
	SET (INSTALL_BINDIR 	bin)
ELSE (WIN32)
	SET (INSTALL_BINDIR 	bin/${PROJECT_NAME})
ENDIF (WIN32)

# for CMake 2.4
SET (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
SET (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

