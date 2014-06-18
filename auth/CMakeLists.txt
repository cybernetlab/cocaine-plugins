IF(AUTH)
    INCLUDE_DIRECTORIES(
        ${PROJECT_SOURCE_DIR}/auth/include)

    LINK_DIRECTORIES()

    ADD_LIBRARY(auth MODULE
        include/cocaine/service/auth.hpp
        src/auth
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
