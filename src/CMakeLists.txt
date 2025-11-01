include(fetch_content_wrapper)
cmake_language(CALL ${PROJECT_NAME}_fetch_content_wrapper
    mdn_status
    GIT_REPOSITORY https://github.com/meni-adin/c-mdn-status.git
    GIT_TAG        main
)
cmake_language(CALL ${PROJECT_NAME}_fetch_content_wrapper
    mdn_mock_wrapper
    GIT_REPOSITORY https://github.com/meni-adin/c-mdn-mock-wrapper.git
    GIT_TAG        main
)
cmake_language(CALL ${PROJECT_NAME}_fetch_content_wrapper
    mdn_logger
    GIT_REPOSITORY https://github.com/meni-adin/c-mdn-logger.git
    GIT_TAG        main
)

# Check for logging level configuration variables using pattern matching
get_cmake_property(ALL_VARS VARIABLES)
foreach(VAR IN LISTS ALL_VARS)
    if(VAR MATCHES "^MDN_CONTAINERS_SET_LOGGING_LEVEL_(.+)$" AND ${VAR})
        set(LEVEL ${CMAKE_MATCH_1})
        set(MDN_CONTAINERS_LOGGER_COMPILE_DEFINITION "MDN_LOGGER_SET_LEVEL_${LEVEL}" CACHE STRING "Logger level override" FORCE)
        message(STATUS "MDN Containers: Overriding logging level to ${LEVEL}")
        break()
    endif()
endforeach()

add_subdirectory(vector)
