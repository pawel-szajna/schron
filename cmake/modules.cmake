function(add_module)
    set(OPTIONS HAS_MOCKS)
    set(VALUE_ARGS NAME TYPE)
    set(LIST_ARGS SOURCES TESTS DEPENDENCIES PUBLIC_DEPENDENCIES INCLUDES)
    cmake_parse_arguments(
        MODULE
        "${OPTIONS}"
        "${VALUE_ARGS}"
        "${LIST_ARGS}"
        ${ARGN})

    list(LENGTH MODULE_SOURCES SOURCES_COUNT)
    if(MODULE_TYPE STREQUAL INTERFACE AND SOURCES_COUNT GREATER 0)
        message(FATAL_ERROR "Module ${MODULE_NAME} has type set to ${MODULE_TYPE}, but was given sources.")
    elseif(NOT MODULE_TYPE STREQUAL INTERFACE AND SOURCES_COUNT EQUAL 0)
        message(FATAL_ERROR "Module ${MODULE_NAME} has type set to ${MODULE_TYPE}, but has no source files.")
    endif()

    if(MODULE_TYPE STREQUAL EXECUTABLE)
        add_executable(
            ${MODULE_NAME}
            ${MODULE_SOURCES}
        )
    else()
        list(TRANSFORM MODULE_SOURCES PREPEND src/)
        add_library(
            ${MODULE_NAME}
            ${MODULE_TYPE}
            ${MODULE_SOURCES}
        )
    endif()

    set_target_properties(
        ${MODULE_NAME}
        PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
    if(${MODULE_TYPE} STREQUAL "INTERFACE")
        target_include_directories(
            ${MODULE_NAME}
            INTERFACE
                include
        )
    else()
        string(TOLOWER ${MODULE_NAME} MODULE_LOWERCASE)
        target_include_directories(
            ${MODULE_NAME}
            PUBLIC
                include
            PRIVATE
                include/${MODULE_LOWERCASE}
                ${MODULE_INCLUDES}
        )
    endif()

    list(LENGTH MODULE_DEPENDENCIES DEPENDENCIES_COUNT)
    list(LENGTH MODULE_PUBLIC_DEPENDENCIES PUBLIC_DEPENDENCIES_COUNT)
    if(DEPENDENCIES_COUNT GREATER 0 OR PUBLIC_DEPENDENCIES_COUNT GREATER 0)
        target_link_libraries(
            ${MODULE_NAME}
            PRIVATE
                ${MODULE_DEPENDENCIES}
            PUBLIC
                ${MODULE_PUBLIC_DEPENDENCIES}
        )
    endif()

#    list(LENGTH MODULE_TESTS TESTS_COUNT)
#    if(TESTS_COUNT GREATER 0 OR MODULE_MOCKS)
#        add_ut(
#                ${MODULE_HAS_MOCKS}
#                TESTED_MODULE ${MODULE_NAME}
#                SOURCES ${MODULE_TESTS}
#        )
#    elseif(NOT MODULE_TYPE STREQUAL EXECUTABLE)
#        message("Notice: module ${MODULE_NAME} has no unit tests defined.")
#    endif()
endfunction()
