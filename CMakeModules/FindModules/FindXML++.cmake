# Search for dependencies
FIND_PACKAGE ( Boost 1.36.0 	REQUIRED )

# Find XML++
SET (WANTED_MODULE_NAME XML++)
SET (WANTED_INCLUDES    xml++/node.h)
SET (WANTED_LIBRARIES   XML++)
INCLUDE (FindCommonModule)
 
