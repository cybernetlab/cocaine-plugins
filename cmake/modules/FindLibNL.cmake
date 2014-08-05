#
# Find the native netlink includes and library
#
# If they exist, differentiate between versions 1, 2 and 3.
# Version 1 does not have netlink/version.h
# Version 3 does have the major version number as a suffix
#   to the libnl name (libnl-3)
#
#  NL_INCLUDE_DIRS - where to find libnl.h, etc.
#  NL_LIBRARIES    - List of libraries when using libnl3.
#  NL_FOUND        - True if libnl found.

IF (LIBNL_LIBRARIES AND LIBNL_INCLUDE_DIRS)
    # in cache already
    SET(LIBNL_FOUND TRUE)
ELSE (LIBNL_LIBRARIES AND LIBNL_INCLUDE_DIRS)
    SET(SEARCHPATHS
        /opt/local
        /sw
        /usr
        /usr/local
    )
    FIND_PATH(LIBNL_INCLUDE_DIR
        PATH_SUFFIXES
            include/libnl3
        NAMES
            netlink/version.h
        PATHS
            $(SEARCHPATHS)
    )
    # NL version >= 2
    IF (LIBNL_INCLUDE_DIR)
        FIND_LIBRARY(LIBNL_LIBRARY
            NAMES
                nl-3 nl
            PATH_SUFFIXES
                lib64 lib
            PATHS
                $(SEARCHPATHS)
        )
        FIND_LIBRARY(LIBNLGENL_LIBRARY
            NAMES
                nl-genl-3 nl-genl
            PATH_SUFFIXES
                lib64 lib
            PATHS
                $(SEARCHPATHS)
        )
        FIND_LIBRARY(LIBNLROUTE_LIBRARY
            NAMES
                nl-route-3 nl-route
            PATH_SUFFIXES
                lib64 lib
            PATHS
                $(SEARCHPATHS)
        )
        #
        # If we don't have all of those libraries, we can't use libnl.
        #
        IF (NOT LIBNLGENL_LIBRARY AND NOT LIBNLROUTE_LIBRARY)
            SET(LIBNL_LIBRARY NOTFOUND)
        ENDIF (NOT LIBNLGENL_LIBRARY AND NOT LIBNLROUTE_LIBRARY)
        IF(LIBNL_LIBRARY)
            STRING(REGEX REPLACE ".*nl-([^.,;]*).*" "\\1" LIBNLSUFFIX ${LIBNL_LIBRARY})
            IF (LIBNLSUFFIX)
                SET(HAVE_LIBNL3 1)
            ELSE (LIBNLSUFFIX)
                SET(HAVE_LIBNL2 1)
            ENDIF (NLSUFFIX)
            SET(HAVE_LIBNL 1)
        ENDIF(LIBNL_LIBRARY)
    ELSE(LIBNL_INCLUDE_DIR)
        # NL version 1 ?
        FIND_PATH(LIBNL_INCLUDE_DIR
            NAMES
                netlink/netlink.h
            PATHS
                $(SEARCHPATHS)
        )
        FIND_LIBRARY(LIBNL_LIBRARY
            NAMES
                nl
            PATH_SUFFIXES
                lib64 lib
            PATHS
                $(SEARCHPATHS)
        )
        IF (LIBNL_INCLUDE_DIR)
            SET(HAVE_LIBNL1 1)
        ENDIF (LIBNL_INCLUDE_DIR)
    ENDIF(LIBNL_INCLUDE_DIR)
ENDIF(LIBNL_LIBRARIES AND LIBNL_INCLUDE_DIRS)
# MESSAGE(STATUS "LIB Found: ${LIBNL_LIBRARY}, Suffix: ${LIBNLSUFFIX}\n  1:${HAVE_LIBNL1}, 2:${HAVE_LIBNL2}, 3:${HAVE_LIBNL3}.")

# handle the QUIETLY and REQUIRED arguments and set LIBNL_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBNL DEFAULT_MSG LIBNL_LIBRARY LIBNL_INCLUDE_DIR)

IF(LIBNL_FOUND)
    SET(LIBNL_LIBRARIES ${LIBNLGENL_LIBRARY} ${LIBNLROUTE_LIBRARY} ${LIBNL_LIBRARY})
    SET(LIBNL_INCLUDE_DIRS ${LIBNL_INCLUDE_DIR})
ELSE()
    SET(LIBNL_LIBRARIES)
    SET(LIBNL_INCLUDE_DIRS)
ENDIF()

MARK_AS_ADVANCED(LIBNL_LIBRARIES LIBNL_INCLUDE_DIRS)
