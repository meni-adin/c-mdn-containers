include(FetchContent)

file(TO_CMAKE_PATH "${CMAKE_BINARY_DIR}/external" FETCH_CONTENT_WRAPPER_EXTERNAL_DIR)

function(${PROJECT_NAME}_content_url_to_local_path url var_prefix)
    # Ensure URL format is as expected
    string(REGEX MATCH "^https://([^/]+)/([^/]+)/([^/]+).git$" whole_url ${url})
    if (NOT "${url}" STREQUAL "${whole_url}")
        message(FATAL_ERROR "Something is wrong with the URL")
    endif()

    # Remove 'https://' from the start
    string(REPLACE "https://" "" url_without_protocol ${url})

    # Extract the domain
    string(REGEX MATCH "^([^/]+)" domain ${url_without_protocol})
    string(REPLACE "${domain}/" "" rest ${url_without_protocol})

    # Extract the user
    string(REGEX MATCH "^([^/]+)" user ${rest})
    string(REPLACE "${user}/" "" rest ${rest})

    # Extract the repo
    string(REGEX REPLACE "^([^/]+)\\.git$" "\\1" repo ${rest})
    string(REPLACE "${repo}/" "" rest ${rest})

    set(local_path "_${domain}/${user}/${repo}")
    file(TO_CMAKE_PATH "${local_path}" local_path)
    set(${var_prefix}_LOCAL_PATH ${local_path} PARENT_SCOPE)
endfunction()

# Note: ARGV0 which sets the name for the external library, has no effect if the function doesn't fall-back to FetchContent.
#       Same for GIT_TAG.
function(${PROJECT_NAME}_fetch_content_wrapper)
    set(options)
    set(oneValueArgs GIT_REPOSITORY GIT_TAG)
    set(multiValueArgs)
    set(allArgs ${options} ${oneValueArgs} ${multiValueArgs})

    cmake_parse_arguments(PARSE_ARGV 0 FETCH_CONTENT_WRAPPER "${options}" "${oneValueArgs}" "${multiValueArgs}")
    if (NOT FETCH_CONTENT_WRAPPER_GIT_REPOSITORY)
        message(FATAL_ERROR "GIT_REPOSITORY argument is required.")
    endif()
    if (NOT FETCH_CONTENT_WRAPPER_GIT_TAG)
        message(FATAL_ERROR "GIT_TAG argument is required.")
    endif()

    cmake_language(CALL ${PROJECT_NAME}_content_url_to_local_path "${FETCH_CONTENT_WRAPPER_GIT_REPOSITORY}" "FETCH_CONTENT_WRAPPER")
    get_filename_component(PROJECT_PARENT_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY)
    file(TO_CMAKE_PATH "${PROJECT_PARENT_DIR}/${FETCH_CONTENT_WRAPPER_LOCAL_PATH}" content_local_path)

    set(content_available OFF)
    if(EXISTS ${content_local_path})
        if(IS_DIRECTORY ${content_local_path})
            message(STATUS "The directory ${content_local_path} exists")
            set(content_available ON)
        else()
            message(STATUS "The path ${content_local_path} exists but is not a directory")
        endif()
    else()
        message(STATUS "The path ${content_local_path} does not exist")
    endif()

    if(content_available)
    file(TO_CMAKE_PATH "${FETCH_CONTENT_WRAPPER_EXTERNAL_DIR}/${FETCH_CONTENT_WRAPPER_LOCAL_PATH}" content_subdirectory_path)
    if(EXISTS ${content_subdirectory_path})
        message(STATUS "Subdirectory ${content_local_path} already added")
    else()
        message(STATUS "Adding ${content_local_path} as a subdirectory")
            add_subdirectory("${content_local_path}" "${content_subdirectory_path}")
        endif()
    else()
        message(STATUS "Falling-back to FetchContent")
        # set(FETCHCONTENT_QUIET FALSE PARENT_SCOPE) # this setting is done at the main CMakeLists.txt file, as here it has no effect
        FetchContent_Declare(
            ${ARGV0}
            GIT_REPOSITORY ${FETCH_CONTENT_WRAPPER_GIT_REPOSITORY}
            GIT_TAG ${FETCH_CONTENT_WRAPPER_GIT_TAG}
            GIT_PROGRESS TRUE
        )
        FetchContent_MakeAvailable(${ARGV0})
    endif()
endfunction()

macro(${PROJECT_NAME}_init_fetch_content_wrapper)
    set(FETCHCONTENT_QUIET FALSE)
    if(("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}") AND (EXISTS "${FETCH_CONTENT_WRAPPER_EXTERNAL_DIR}"))
        message(STATUS "Removing ${FETCH_CONTENT_WRAPPER_EXTERNAL_DIR} left from previous run")
        file(REMOVE_RECURSE "${FETCH_CONTENT_WRAPPER_EXTERNAL_DIR}")
    endif()
endmacro()
