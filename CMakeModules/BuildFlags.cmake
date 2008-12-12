IF (CMAKE_COMPILER_IS_GNUCC)
	# By default used -O3 which may be dangerous, so, change to -O2
	SET (CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
	SET (CMAKE_CXX_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})

	ADD_DEFINITIONS (-Wall)
	ADD_DEFINITIONS (-Wformat=2)
	ADD_DEFINITIONS (-Winit-self)
#	Disabled because really a lot of warnings even if "default" presented.
#	Instead using -Wswitch which doesn't produce warning when "default" presented.
#	-Wswitch is turned on by -Wall
# 	ADD_DEFINITIONS (-Wswitch-enum)
# 	Disabled because of Boost.Test
# 	ADD_DEFINITIONS (-Wold-style-cast)
	ADD_DEFINITIONS (-Woverloaded-virtual)
	ADD_DEFINITIONS (-Wsign-promo)
	ADD_DEFINITIONS (-Wunused-parameter)
	ADD_DEFINITIONS (-Wextra)
# 	Disabled because of EVmath
# 	ADD_DEFINITIONS (-Wfloat-equal)
	ADD_DEFINITIONS (-Wundef)
# 	Disabled because of ';' after SET_LOGGER
# 	ADD_DEFINITIONS (-pedantic)


	IF (CMAKE_SIZEOF_VOID_P EQUAL 8)
		# 64-bit architecture
		# Need to use -fPIC for even static libraries, if it is
		# planning to use this library linked to dynamic libraries
		ADD_DEFINITIONS (-fPIC)
	ENDIF (CMAKE_SIZEOF_VOID_P EQUAL 8)
ENDIF (CMAKE_COMPILER_IS_GNUCC)

IF (MSVC)
	ADD_DEFINITIONS (-D_CRT_SECURE_NO_WARNINGS)
	ADD_DEFINITIONS (-D_CRT_SECURE_NO_DEPRECATE)
	ADD_DEFINITIONS (-D_SCL_SECURE_NO_WARNINGS)
	ADD_DEFINITIONS (-DBOOST_ALL_NO_LIB)
ENDIF (MSVC)

# Building shared libs by default
SET (BUILD_SHARED_LIBS SHARED)
# Building in RELEASE by default
IF (NOT CMAKE_BUILD_TYPE)
	SET (CMAKE_BUILD_TYPE RELEASE)
	SET (CMAKE_BUILD_TYPE CACHE STRING "Build type")
ENDIF (NOT CMAKE_BUILD_TYPE)
