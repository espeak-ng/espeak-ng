# Usage: msys2_to_windows("${msys_path}" win_path)

function(msys_to_windows MsysPath WinPath)
    execute_process(
            COMMAND cygpath -w "${MsysPath}"
            OUTPUT_VARIABLE converted_path
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_VARIABLE err
            RESULT_VARIABLE res
    )
    if(NOT res EQUAL 0)
        message(WARNING "cygpath failed: ${err}")
        set(${WinPath} "${MsysPath}" PARENT_SCOPE)
    else ()
        string(REPLACE "\\" "/" norm_path "${converted_path}")
        set(${WinPath} "${norm_path}" PARENT_SCOPE)
    endif ()
endfunction()