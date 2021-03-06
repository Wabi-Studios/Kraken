if "%NOBUILD%"=="1" goto EOF
echo %TIME% > %BUILD_DIR%\buildtime.txt
msbuild ^
	%BUILD_DIR%\Kraken.sln ^
	/target:build ^
	/property:Configuration=%BUILD_TYPE% ^
	/p:PreferredToolArchitecture=x64 ^
	/maxcpucount ^
	/verbosity:minimal ^
	/p:platform=%MSBUILD_PLATFORM% ^
	/flp:Summary;Verbosity=minimal;LogFile=%BUILD_DIR%\Build.log 
	if errorlevel 1 (
		echo Error during build, see %BUILD_DIR%\Build.log for details 
		exit /b 1
	)

msbuild ^
	%BUILD_DIR%\INSTALL.vcxproj ^
	/property:Configuration=%BUILD_TYPE% ^
	/p:PreferredToolArchitecture=x64 ^
	/maxcpucount ^
	/verbosity:minimal ^
	/p:platform=%MSBUILD_PLATFORM% 
	if errorlevel 1 (
		echo Error during install phase
		exit /b 1
	)
echo %TIME% >> %BUILD_DIR%\buildtime.txt
:EOF