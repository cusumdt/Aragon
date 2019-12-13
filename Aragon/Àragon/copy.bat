:: Copia de archivos
:: Evento posterior a la compilacion |||  $(ProjectDir)Copy.bat "$(SolutionDir)" "$(TargetDir)"  |||
::@echo off

echo Copiando archivos...

set SolutionDir=%1
set TargetDir=%2

echo %TargetDir%
echo %SolutionDir%

MKDIR "%TargetDir%res"
XCOPY "%SolutionDir%res" "%TargetDir%res" /E /Y /S /H


