# Usage: msys2_to_windows("${msys_path}" win_path)

function(msys_to_windows MsysPath WinPath)
    execute_process(
            COMMAND cygpath -w "${MsysPath}"
            OUTPUT_VARIABLE converted_path
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endfunction()