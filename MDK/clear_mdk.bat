@echo off
echo ========================================
echo   Clear MDK Project Temp Files
echo ========================================
echo.

set "PROJECT_ROOT=%~dp0"
echo Project path: %PROJECT_ROOT%
echo.

echo Cleaning up:
echo   *.scvd
echo   *.uvoptx
echo   *.uvguix.*
echo.

set COUNT=0

for %%f in ("%PROJECT_ROOT%*.scvd") do (
    echo Delete: %%f
    del "%%f" /f /q
    set /a COUNT+=1
)

for %%f in ("%PROJECT_ROOT%*.uvoptx") do (
    echo Delete: %%f
    del "%%f" /f /q
    set /a COUNT+=1
)

for %%f in ("%PROJECT_ROOT%*.uvguix.*") do (
    echo Delete: %%f
    del "%%f" /f /q
    set /a COUNT+=1
)

echo.
echo ========================================
echo Done! Deleted %COUNT% files
echo ========================================
pause
