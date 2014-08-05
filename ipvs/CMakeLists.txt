IF(IPVS)
    # Probably we should place it in root CMakeFile.txt
    SET(COCAINE_VERSION_MAJOR   0)
    SET(COCAINE_VERSION_MINOR   11)
    SET(COCAINE_VERSION_RELEASE 0)
    CONFIGURE_FILE(
        "${PROJECT_SOURCE_DIR}/ipvs/config.hpp.in"
        "${PROJECT_SOURCE_DIR}/ipvs/include/cocaine/config.hpp")

    ADD_SUBDIRECTORY(libipvs-1.26)

#    LOCATE_LIBRARY(LIBNL "libnl3/netlink/netlink.h" "libnl")
#    FIND_PACKAGE(LibNL)

    INCLUDE_DIRECTORIES(
        ${PROJECT_SOURCE_DIR}/ipvs/include
        ${PROJECT_SOURCE_DIR}/ipvs/libipvs-1.26
        ${LIBNL_INCLUDE_DIRS})

    LINK_DIRECTORIES(
        ${LIBNL_LIBRARY_DIRS})

    ADD_LIBRARY(ipvs-gateway MODULE
        src/gateway
        src/module)

    TARGET_LINK_LIBRARIES(ipvs-gateway
        cocaine-core
        ipvs)

    SET_TARGET_PROPERTIES(ipvs-gateway PROPERTIES
        PREFIX ""
        SUFFIX ".cocaine-plugin"
        COMPILE_FLAGS "-std=c++0x")

    INSTALL(TARGETS ipvs-gateway
        LIBRARY DESTINATION lib/cocaine
        COMPONENT runtime)

    INSTALL(
        DIRECTORY
            include/cocaine
        DESTINATION include/cocaine
        COMPONENT development)
ENDIF(IPVS)
