if EXIST %PYTHON% (
	goto detect_python_done
)

echo python not found in lib folder
exit /b 1

:detect_python_done

REM Use -B to avoid writing __pycache__ in lib directory and causing update conflicts.
%PYTHON% -B %KRAKEN_DIR%\build_files\utils\make_update.py --git-command "%GIT%" --svn-command "%SVN%" %BUILD_UPDATE_ARGS%

:EOF