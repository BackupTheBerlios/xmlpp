SET (HEADER_PATH ${PROJECT_SOURCE_DIR}/xml++)
SET (TARGET_HEADERS
	${HEADER_PATH}/attribute.h
	${HEADER_PATH}/document.h
	${HEADER_PATH}/element.h
	${HEADER_PATH}/iterators.hpp
	${HEADER_PATH}/node.h
	${HEADER_PATH}/tinyxml.h
)

SET (TARGET_SERIALIZATION_HEADERS
	${HEADER_PATH}/serialization/archive.hpp
	${HEADER_PATH}/serialization/attribute_serializer.hpp
	${HEADER_PATH}/serialization/container_serializer.hpp
	${HEADER_PATH}/serialization/custom_serializer.hpp
	${HEADER_PATH}/serialization/element_serializer.hpp
	${HEADER_PATH}/serialization/generic_serializer.hpp
	${HEADER_PATH}/serialization/helpers.hpp
	${HEADER_PATH}/serialization/name_value_pair.hpp
	${HEADER_PATH}/serialization/serialization.hpp
	${HEADER_PATH}/serialization/text_serializer.hpp
)

SOURCE_GROUP( headers FILES 				${TARGET_HEADERS} )
SOURCE_GROUP( headers\\serialization FILES 	${TARGET_SERIALIZATION_HEADERS} )

SET (TARGET_SOURCES
	attribute.cpp
	document.cpp
	element.cpp
	node.cpp
	tinyxml.cpp
	tinyxmlerror.cpp
	tinyxmlparser.cpp
)

SOURCE_GROUP( sources FILES	 ${TARGET_SOURCES} )

SET (TARGET_SOURCES
	${TARGET_HEADERS}
	${TARGET_SOURCES}
	${TARGET_SERIALIZATION_HEADERS}
)
	
ADD_DEFINITIONS (
	-DTIXML_USE_STL
)

IF (MSVC)
	ADD_DEFINITIONS (-D_CRT_SECURE_NO_WARNINGS)
	ADD_DEFINITIONS (-D_CRT_SECURE_NO_DEPRECATE)
	ADD_DEFINITIONS (-D_SCL_SECURE_NO_WARNINGS)
ENDIF(MSVC)

ADD_LIBRARY( ${TARGET_NAME} STATIC ${TARGET_SOURCES} ${TARGET_OPTIONS} )

IF (XMLPP_CONFIGURE_INTRUSIVE)
    SET_TARGET_PROPERTIES ( ${TARGET_NAME} PROPERTIES 
        FOLDER "${XMLPP_PROJECT_GROUP}"
    )
ENDIF (XMLPP_CONFIGURE_INTRUSIVE)

IF (NOT XMLPP_CONFIGURE_INTRUSIVE)
	# install
	INSTALL(
		TARGETS ${TARGET_NAME}
		RUNTIME DESTINATION ${INSTALL_BINDIR}
		LIBRARY DESTINATION ${INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${INSTALL_ARCHIVEDIR}
	)

	INSTALL(
		FILES        ${TARGET_HEADERS}
		DESTINATION  ${INSTALL_INCDIR}/xml++
	)

	INSTALL (
		FILES       ${TARGET_SERIALIZATION_HEADERS}
		DESTINATION ${INSTALL_INCDIR}/xml++/serialization
	)
ENDIF (NOT XMLPP_CONFIGURE_INTRUSIVE)