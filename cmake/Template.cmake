# cmake/Template.cmake

# Token substitution helper
function(_tmpl_substitute content_var type name upper_name type_inc self_inc project_upper)
    set(_content "${${content_var}}")

    string(REPLACE "$TYPE$" "${type}" _content "${_content}")
    string(REPLACE "$NAME$" "${name}" _content "${_content}")
    string(REPLACE "$UPPER_NAME$" "${upper_name}" _content "${_content}")
    string(REPLACE "$TYPE_INCLUDE$" "${type_inc}" _content "${_content}")
    string(REPLACE "$SELF_INCLUDE$" "${self_inc}" _content "${_content}")
    string(REPLACE "$PROJECT$" "${project_upper}" _content "${_content}")

    set(${content_var} "${_content}" PARENT_SCOPE)
endfunction()


function(_write_and_format filepath content)
    if(EXISTS "${filepath}")
        file(READ "${filepath}" _existing)
        if(_existing STREQUAL content)
            return()
        endif()
    endif()

    file(WRITE "${filepath}" "${content}")

    if(DEFINED _CLANG_FORMAT_AVAILABLE)
        execute_process(
            COMMAND "${CLANG_FORMAT}" -i --style=file "${filepath}"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            RESULT_VARIABLE _result
            ERROR_QUIET OUTPUT_QUIET
        )
        if(NOT _result EQUAL 0)
            message(WARNING "clang-format failed on: ${filepath}")
        endif()
    endif()
endfunction()


#[[
template_instantiate(
    HEADER          <template.h.in>
    [SOURCE         <template.c.in>]
    [HEADERS        <list_var>}
    [SOURCES        <list_var>]
    [SUBDIR         <subdir>]          # e.g. "client"  → puts files in generated/inc/client/
    
    # Single mode
    [NAME           <name>]
    [TYPE           <type>]
    [TYPE_INCLUDE   <header>]
    [OUTPUT_BASE    <name>]

    # Batch mode
    [ITEMS
        <name> <type> [type_include]
        ...
    ]
)
]]
function(template_instantiate)
    cmake_parse_arguments(PARSE_ARGV 0 ARG ""
        "HEADER;SOURCE;HEADERS;SOURCES;SUBDIR;NAME;TYPE;TYPE_INCLUDE;OUTPUT_BASE"
        "ITEMS"
    )

    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "template_instantiate: unknown arguments: ${ARG_UNPARSED_ARGUMENTS}")
    endif()

    if(NOT ARG_HEADER)
        message(FATAL_ERROR "template_instantiate: HEADER is required")
    endif()

    # Batch mode
    if(ARG_ITEMS)
        list(LENGTH ARG_ITEMS len)
        foreach(i RANGE 0 ${len} 3)
            if(i GREATER_EQUAL len)
                break()
            endif()

            list(GET ARG_ITEMS ${i} name)
            math(EXPR type_idx "${i} + 1")
            list(GET ARG_ITEMS ${type_idx} type)

            set(type_include "")
            math(EXPR inc_idx "${i} + 2")
            if(inc_idx LESS len)
                list(GET ARG_ITEMS ${inc_idx} possible_inc)
                if(possible_inc MATCHES "\.[hH]$" OR possible_inc MATCHES "/")
                    set(type_include "${possible_inc}")
                endif()
            endif()

            template_instantiate(
                HEADER "${ARG_HEADER}"
                SOURCE "${ARG_SOURCE}"
                SUBDIR "${ARG_SUBDIR}"
                NAME "${name}"
                TYPE "${type}"
                TYPE_INCLUDE "${type_include}"
                OUTPUT_BASE "${ARG_OUTPUT_BASE}"
                HEADERS ${ARG_HEADERS}
                SOURCES ${ARG_SOURCES}
            )
        endforeach()
        return()
    endif()

    # ==================== Single instantiation ====================

    if(NOT ARG_NAME OR NOT ARG_TYPE)
        message(FATAL_ERROR "template_instantiate: NAME and TYPE are required")
    endif()

    # One-time setup
    if(NOT DEFINED _TEMPLATE_INSTANTIATE_INITIALIZED)
        set(_TEMPLATE_INSTANTIATE_INITIALIZED TRUE CACHE INTERNAL "")

        find_program(CLANG_FORMAT clang-format QUIET)
        if(CLANG_FORMAT AND EXISTS "${CMAKE_SOURCE_DIR}/.clang-format")
            set(_CLANG_FORMAT_AVAILABLE TRUE CACHE INTERNAL "")
            message(STATUS "template_instantiate: clang-format enabled")
        else()
            message(STATUS "template_instantiate: clang-format disabled")
        endif()
    endif()

    # Dependency tracking
    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND PROPERTY
        CMAKE_CONFIGURE_DEPENDS "${ARG_HEADER}")
    if(ARG_SOURCE)
        set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND PROPERTY
            CMAKE_CONFIGURE_DEPENDS "${ARG_SOURCE}")
    endif()

    # === Naming & Paths ===
    cmake_path(GET ARG_HEADER STEM LAST_ONLY _base_name)
    if(ARG_OUTPUT_BASE)
        set(_base_name "${ARG_OUTPUT_BASE}")
    endif()

    string(TOUPPER "${ARG_NAME}" _upper_name)
    string(TOUPPER "${PROJECT_NAME}" _upper_project)

    # Subdirectory support
    set(_inc_subdir "")
    set(_self_subdir "")
    if(ARG_SUBDIR)
        set(_inc_subdir "/${ARG_SUBDIR}")
        set(_self_subdir "${ARG_SUBDIR}/")
    endif()

    # Auto-generated includes
    set(_type_include "")
    if(ARG_TYPE_INCLUDE)
        set(_type_include "#include \"${ARG_TYPE_INCLUDE}\"")
    endif()

    set(_self_include "#include \"${_self_subdir}${_base_name}_${ARG_NAME}.h\"")

    set(_out_inc "${CMAKE_CURRENT_BINARY_DIR}/generated/inc")
    set(_out_src "${CMAKE_CURRENT_BINARY_DIR}/generated/src")

    file(MAKE_DIRECTORY "${_out_inc}${_inc_subdir}" "${_out_src}")

    # Generate Header
    file(READ "${ARG_HEADER}" _header_content)
    _tmpl_substitute(_header_content
        "${ARG_TYPE}" "${ARG_NAME}" "${_upper_name}"
        "${_type_include}" "${_self_include}" "${_upper_project}"
    )

    set(_header_out "${_out_inc}${_inc_subdir}/${_base_name}_${ARG_NAME}.h")
    _write_and_format("${_header_out}" "${_header_content}")

    if(ARG_HEADERS)
        list(APPEND ${ARG_HEADERS} "${_header_out}")
        set(${ARG_HEADERS} "${${ARG_HEADERS}}" PARENT_SCOPE)
    endif()

    # Generate Source
    if(ARG_SOURCE)
        file(READ "${ARG_SOURCE}" _source_content)
        _tmpl_substitute(_source_content
            "${ARG_TYPE}" "${ARG_NAME}" "${_upper_name}"
            "${_type_include}" "${_self_include}" "${_upper_project}"
        )

        set(_source_out "${_out_src}/${_base_name}_${ARG_NAME}.c")
        _write_and_format("${_source_out}" "${_source_content}")

        if(ARG_SOURCES)
            list(APPEND ${ARG_SOURCES} "${_source_out}")
            set(${ARG_SOURCES} "${${ARG_SOURCES}}" PARENT_SCOPE)
        endif()
    endif()
endfunction()
