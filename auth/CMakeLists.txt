IF(AUTH)
    INCLUDE_DIRECTORIES(
        ${PROJECT_SOURCE_DIR}/auth/include
        ${PROJECT_SOURCE_DIR}/auth/md5
        ${PROJECT_SOURCE_DIR}/auth/unique_id)

    LINK_DIRECTORIES()

    ADD_LIBRARY(auth MODULE
        # include/cocaine/service/auth.hpp
        unique_id/unique_id
        md5/md5
        src/auth
        src/realm
        src/storage/abstract_storage
        src/storage/cocaine_storage
        src/storage/memory_storage
        src/authentication/plain
        src/authentication/md5
        src/module)

    TARGET_LINK_LIBRARIES(auth
        cocaine-core)

    SET_TARGET_PROPERTIES(auth PROPERTIES
        PREFIX ""
        SUFFIX ".cocaine-plugin"
        COMPILE_FLAGS "-std=c++0x")

    INSTALL(TARGETS auth
        LIBRARY DESTINATION lib/cocaine
        COMPONENT runtime)

    INSTALL(
        DIRECTORY
            include/
        DESTINATION include
        COMPONENT development)
ENDIF(AUTH)
