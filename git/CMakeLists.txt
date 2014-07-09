IF(GIT)
    INCLUDE(cmake/FindLibgit2.cmake)

    INCLUDE_DIRECTORIES(
        ${PROJECT_SOURCE_DIR}/git/include
        ${LIBGIT2_INCLUDE_DIRS})
#        ${PROJECT_SOURCE_DIR}/git/unique_id)

    LINK_DIRECTORIES()

    ADD_LIBRARY(git MODULE
        # unique_id/unique_id
        # md5/md5
        src/git
        src/cocaine_odb_backend
        src/repository
        # src/storage/cocaine_storage
        # src/storage/memory_storage
        # src/authentication/plain
        # src/authentication/md5
        src/module)

    TARGET_LINK_LIBRARIES(git
        cocaine-core
        ${LIBGIT2_LIBRARIES})

    SET_TARGET_PROPERTIES(git PROPERTIES
        PREFIX ""
        SUFFIX ".cocaine-plugin"
        COMPILE_FLAGS "-std=c++0x")

    INSTALL(TARGETS git
        LIBRARY DESTINATION lib/cocaine
        COMPONENT runtime)

    INSTALL(
        DIRECTORY
            include/
        DESTINATION include
        COMPONENT development)
ENDIF(GIT)
