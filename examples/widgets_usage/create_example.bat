@echo off
setlocal enabledelayedexpansion

if "%~1"=="" (
    echo Uso: create_examples.bat NUMERO
    exit /b
)

set MAX=%1

for /l %%i in (1,1,%MAX%) do (

    set num=%%i

    if %%i LSS 10 (
        set num=0%%i
    )

    set folder=example_!num!
    set file=example_!num!.ino

    if exist "!folder!" (
        echo Pasta !folder! ja existe, pulando...
    ) else (
        mkdir "!folder!"
        echo. > "!folder!\!file!"
        echo Criado !folder!\!file!
    )
)

echo Concluido.