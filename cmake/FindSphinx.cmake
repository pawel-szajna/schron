include(FindPackageHandleStandardArgs)

find_package(PythonInterp)
if(PYTHONINTERP_FOUND)
    get_filename_component(_PYTHON_DIR "${PYTHON_EXECUTABLE}" DIRECTORY)
endif()

function(find_python_app)
    set(VALUE_ARGS NAME VARIABLE)
    cmake_parse_arguments(APP "" "${VALUE_ARGS}" "" ${ARGN})

    find_program(
        ${APP_VARIABLE}
        NAMES ${APP_NAME}
        HINTS
            "${_PYTHON_DIR}"
            "${_PYTHON_DIR}/bin"
            "${_PYTHON_DIR}/Scripts"
    )
    if(${${APP_VARIABLE}} MATCHES "NOTFOUND")
        execute_process(
            COMMAND "${PYTHON_EXECUTABLE}" "-m" "site" "--user-base"
            OUTPUT_VARIABLE _PYTHON_USER_BASE
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
            TIMEOUT 1
        )
        find_program(
            ${APP_VARIABLE}
            NAMES ${APP_NAME}
            HINTS
                "${_PYTHON_USER_BASE}/bin"
        )
    endif()
endfunction()

find_python_app(
    NAME sphinx-build
    VARIABLE SPHINX_BUILD_EXECUTABLE
)

find_python_app(
    NAME breathe-apidoc
    VARIABLE BREATHE_APIDOC_EXECUTABLE
)

find_package_handle_standard_args(
    Sphinx
    "Failed to find sphinx/breathe"
    SPHINX_BUILD_EXECUTABLE
    BREATHE_APIDOC_EXECUTABLE
)
