# Performs token substitution on a template string.
function(_tmpl_substitute var_name arg_type arg_name upper_name type_inc self_inc project_upper)
    set(_content "${${var_name}}")
    string(REPLACE "$TYPE$" "${arg_type}" _content "${_content}")
    string(REPLACE "$NAME$" "${arg_name}" _content "${_content}")
    string(REPLACE "$UPPER_NAME$" "${upper_name}" _content "${_content}")
    string(REPLACE "$TYPE_INCLUDE$" "${type_inc}" _content "${_content}")
    string(REPLACE "$SELF_INCLUDE$" "${self_inc}" _content "${_content}")
    string(REPLACE "$PROJECT$" "${project_upper}" _content "${_content}")
    set(${var_name} "${_content}" PARENT_SCOPE)
endfunction()

# Helper: write file only if changed, and format with clang-format only if .clang-format exists
function(_write_and_format path content)
    # Write only if content actually changed
    if(EXISTS "${path}")
        file(READ "${path}" _existing)
        if(_existing STREQUAL content)
            return()
        endif()
    endif()

    file(WRITE "${path}" "${content}")

    # Format ONLY if clang-format is found AND .clang-format exists in repo root
    if(CLANG_FORMAT AND EXISTS "${CMAKE_SOURCE_DIR}/.clang-format")
        execute_process(
            COMMAND "${CLANG_FORMAT}" -i --style=file "${path}"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            RESULT_VARIABLE _format_result
            ERROR_QUIET
            OUTPUT_QUIET
        )
        if(NOT _format_result EQUAL 0)
            message(WARNING "clang-format failed on ${path}, continuing anyway")
        endif()
    endif()
endfunction()

# target_template_instantiate(<target>
#     HEADER         <path>         Path to the template .h file.
#     [SOURCE        <path>]        Path to the template .c file. When omitted
#                                   the generated target is header-only.
#     NAME           <suffix>       Suffix appended to the output filename and
#                                   used as the $NAME$ token, e.g. "pchunk_job".
#     TYPE           <C type>       C type expression used as the $TYPE$ token,
#                                   e.g. "float" or "chunk_job_t *".
#     [TYPE_INCLUDE  <header>]      Header that defines TYPE, e.g.
#                                   "client/chunk_job.h". Expands to an
#                                   #include directive as $TYPE_INCLUDE$.
#     [LINK_LIBRARIES <lib> ...]    Extra libraries linked into the generated
#                                   target as PUBLIC.
# )
#
# Tokens substituted in both the header and source template:
#   $TYPE$          -> TYPE
#   $NAME$          -> NAME
#   $UPPER_NAME$    -> NAME uppercased (for include guards)
#   $TYPE_INCLUDE$  -> TYPE_INCLUDE  (empty when omitted)
#   $SELF_INCLUDE$  -> BASENAME_NAME.h
#   $PROJECT$       -> PROJECT_NAME uppercased
#
# Output files (under ${CMAKE_CURRENT_BINARY_DIR}/generated/):
#   inc/<target>/BASENAME_NAME.h
#   src/BASENAME_NAME.c            only when SOURCE is provided
#
# Created target:  ${PROJECT_NAME}_BASENAME_NAME
#   INTERFACE  when SOURCE is omitted
#   OBJECT     when SOURCE is provided
function(target_template_instantiate target)
    cmake_parse_arguments(PARSE_ARGV 1 ARG "" "HEADER;SOURCE;NAME;TYPE;TYPE_INCLUDE" "LINK_LIBRARIES")

    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "target_template_instantiate: unexpected arguments: ${ARG_UNPARSED_ARGUMENTS}")
    endif()
    if(NOT ARG_HEADER OR NOT ARG_NAME OR NOT ARG_TYPE)
        message(FATAL_ERROR "target_template_instantiate: HEADER, NAME, and TYPE are required")
    endif()

    # Find clang-format once (cached) and inform user
    if(NOT DEFINED CLANG_FORMAT)
        find_program(CLANG_FORMAT clang-format)
        if(CLANG_FORMAT)
            if(EXISTS "${CMAKE_SOURCE_DIR}/.clang-format")
                message(STATUS "Found clang-format + .clang-format - generated files will be formatted")
            else()
                message(STATUS "clang-format found but no .clang-format in source root - skipping formatting")
            endif()
        else()
            message(STATUS "clang-format not found - generated files will not be formatted")
        endif()
    endif()

    # Re-run cmake automatically when either template file changes.
    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND PROPERTY
        CMAKE_CONFIGURE_DEPENDS "${ARG_HEADER}")
    if(ARG_SOURCE)
        set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND PROPERTY
            CMAKE_CONFIGURE_DEPENDS "${ARG_SOURCE}")
    endif()

    cmake_path(GET ARG_HEADER STEM _base)
    string(TOUPPER "${ARG_NAME}" _upper)
    string(TOUPPER "${PROJECT_NAME}" _upper_project)

    set(_type_inc "")
    if(ARG_TYPE_INCLUDE)
        set(_type_inc "${ARG_TYPE_INCLUDE}")
    endif()
    set(_self_inc "${target}/${_base}_${ARG_NAME}.h")

    set(_out_inc "${CMAKE_CURRENT_BINARY_DIR}/generated/inc")
    set(_out_src "${CMAKE_CURRENT_BINARY_DIR}/generated/src")
    set(_tgt "${PROJECT_NAME}_${_base}_${ARG_NAME}")

    file(MAKE_DIRECTORY "${_out_inc}/${target}" "${_out_src}")

    # Generate header
    file(READ "${ARG_HEADER}" _h)
    _tmpl_substitute(_h "${ARG_TYPE}" "${ARG_NAME}" "${_upper}" "${_type_inc}" "${_self_inc}" "${_upper_project}")
    set(_h_out "${_out_inc}/${target}/${_base}_${ARG_NAME}.h")
    _write_and_format("${_h_out}" "${_h}")

    if(ARG_SOURCE)
        # Generate source and create a compiled OBJECT target.
        file(READ "${ARG_SOURCE}" _c)
        _tmpl_substitute(_c "${ARG_TYPE}" "${ARG_NAME}" "${_upper}" "${_type_inc}" "${_self_inc}" "${_upper_project}")
        set(_c_out "${_out_src}/${_base}_${ARG_NAME}.c")
        _write_and_format("${_c_out}" "${_c}")

        add_library("${_tgt}" OBJECT "${_c_out}")
        target_include_directories("${_tgt}" PUBLIC "${_out_inc}")
        if(ARG_LINK_LIBRARIES)
            target_link_libraries("${_tgt}" PUBLIC ${ARG_LINK_LIBRARIES})
        endif()
    else()
        add_library("${_tgt}" INTERFACE)
        target_include_directories("${_tgt}" INTERFACE "${_out_inc}")
    endif()

    target_link_libraries("${target}" PRIVATE "${_tgt}")
endfunction()
