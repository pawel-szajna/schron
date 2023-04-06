if(DISABLE_DOCS)
    return()
endif()

message("
Documentation is enabled. This requires Doxygen, Sphinx,
and Breathe up and running in your system. If you do not
wish to generate the docs, use -DDISABLE_DOCS=ON with cmake.

Additionally, the target docs_pdf requires LaTeX.
")

find_package(Doxygen REQUIRED)
find_package(Sphinx REQUIRED)

set(DOCS_INTERMEDIATE_DIRECTORY ${CMAKE_BINARY_DIR}/src/docs)
set(DOCS_TARGET_DIRECTORY ${CMAKE_BINARY_DIR}/docs)
set(DOXYGEN_INPUT ${PROJECT_SOURCE_DIR}/src)
set(DOXYGEN_OUTPUT ${DOCS_INTERMEDIATE_DIRECTORY}/doxygen)
set(DOXYGEN_INDEX ${DOXYGEN_OUTPUT}/xml/index.xml)
set(SPHINX_DIRECTORY ${DOCS_INTERMEDIATE_DIRECTORY}/sphinx)
set(SPHINX_CLASS_INDEX ${SPHINX_DIRECTORY}/classlist.rst)

file(GLOB_RECURSE ALL_HPP_FILES ${PROJECT_SOURCE_DIR}/src/*.hpp)
file(GLOB_RECURSE ALL_RST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.rst)
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
    ${DOCS_INTERMEDIATE_DIRECTORY}/Doxyfile
    @ONLY
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/conf.py
    ${SPHINX_DIRECTORY}/conf.py
    COPYONLY
)

add_custom_command(
    OUTPUT
        ${DOXYGEN_INDEX}
    DEPENDS
        ${ALL_HPP_FILES}
    COMMAND
        ${DOXYGEN_EXECUTABLE}
        ${DOCS_INTERMEDIATE_DIRECTORY}/Doxyfile
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}
    MAIN_DEPENDENCY
        Doxyfile
)

add_custom_command(
    OUTPUT
        ${SPHINX_CLASS_INDEX}
    DEPENDS
        ${DOXYGEN_INDEX}
    COMMAND
        ${BREATHE_APIDOC_EXECUTABLE}
        -fmqo ${SPHINX_DIRECTORY}
        ${DOXYGEN_OUTPUT}/xml
)

set(TARGET_RST_LOCATIONS "")
foreach(RST_FILE IN LISTS ALL_RST_FILES)
    string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR} ${SPHINX_DIRECTORY} TARGET_RST ${RST_FILE})
    list(APPEND TARGET_RST_LOCATIONS ${TARGET_RST})
    add_custom_command(
        OUTPUT
            ${TARGET_RST}
        DEPENDS
            ${RST_FILE}
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different ${RST_FILE} ${TARGET_RST}
        COMMENT
            "Prepare RST source ${RST_FILE} in build directory"
    )
endforeach()

add_custom_target(
    docs_html
    DEPENDS
        ${SPHINX_CLASS_INDEX}
        ${TARGET_RST_LOCATIONS}
    COMMAND
        ${SPHINX_BUILD_EXECUTABLE}
        -b html
        ${SPHINX_DIRECTORY}
        ${DOCS_TARGET_DIRECTORY}
        -Dbreathe_projects.schron=${DOXYGEN_OUTPUT}/xml
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(
    docs_pdf
    DEPENDS
        ${SPHINX_CLASS_INDEX}
        ${TARGET_RST_LOCATIONS}
    COMMAND
        ${SPHINX_BUILD_EXECUTABLE}
        -M latexpdf
        ${SPHINX_DIRECTORY}
        ${DOCS_TARGET_DIRECTORY}
        -Dbreathe_projects.schron=${DOXYGEN_OUTPUT}/xml
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(
    docs
    DEPENDS
        docs_html
        docs_pdf
    COMMAND
        true
)
