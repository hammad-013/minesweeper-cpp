@echo off
mkdir "build\win" 2>nul
copy "C:\raylib\lib\raylib.dll" "build\win\" >nul
del build\win\main.exe
g++ src\main.cpp ^
    -IC:\raylib\include ^
    -Lbuild/win ^
    -g -ggdb ^
    -lraylib -lgdi32 -lwinmm -lole32 ^
    -o build/win/main.exe ^
    -std=c++17 -pedantic

if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo   Build succeeded! (C++)
    echo   Executable: build\win\main.exe
    echo ========================================
    echo.
    @REM build\win\main.exe
    exit /b 0
) else (
    echo.
    echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    echo   BUILD FAILED! (error code %errorlevel%)
    echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    echo.
    exit /b %errorlevel%
)