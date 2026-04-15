@echo off
echo Deletando arquivos .json em %cd% e subpastas...
for /r %%f in (*.json) do (
    del /f /q "%%f"
    echo Deletado: %%f
)
echo Concluido.
pause