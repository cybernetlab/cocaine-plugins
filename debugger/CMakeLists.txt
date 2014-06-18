IF(DEBUGGER)
#    find_package(Boost COMPONENTS system thread REQUIRED)

    INCLUDE_DIRECTORIES(
        ${PROJECT_SOURCE_DIR}/debugger/include)

#    LINK_DIRECTORIES(
#        ${LIBSWARM_LIBRARY_DIRS})

    ADD_LIBRARY(debugger MODULE
        include/cocaine/service/debugger.hpp
        include/cocaine/idl/debugger.hpp
        src/debugger
        src/module)

    TARGET_LINK_LIBRARIES(debugger
        cocaine-core
#        swarm
#        swarm_urlfetcher
#        ${Boost_LIBRARIES}
    )

    SET_TARGET_PROPERTIES(debugger PROPERTIES
        PREFIX ""
        SUFFIX ".cocaine-plugin"
        COMPILE_FLAGS "-std=c++0x")

    INSTALL(TARGETS debugger
        LIBRARY DESTINATION lib/cocaine
        COMPONENT runtime)

    INSTALL(
        DIRECTORY
            include/cocaine/idl
        DESTINATION include/cocaine
        COMPONENT development)
ENDIF(DEBUGGER)
