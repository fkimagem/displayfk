@echo off
cls
echo Executar esse comando ira remover todas as alteracoes locais dessa biblioteca.
set /p resposta="Deseja continuar? (y/n): "

if /i "%resposta%"=="y" (
    echo.
    echo Atualizando repositorio...
    git fetch --all
    git reset --hard origin/main
    echo.
    echo ============================================
    echo Repositorio atualizado com sucesso!
    echo ============================================
) else (
    echo.
    echo ============================================
    echo Operacao cancelada. Nenhuma alteracao feita.
    echo ============================================
)

echo.
pause
