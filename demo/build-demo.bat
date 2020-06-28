@echo off

set buildOutputFile=build

if exist .\%buildOutputFile% (
echo del %buildOutputFile%
del %buildOutputFile%
) else (
echo mkdir %buildOutputFile%
mkdir %buildOutputFile%
)

cd %buildOutputFile%
echo.

echo start... "call cmake ."
call cmake-gui ..\.
echo.

echo %cd%\indemind_demos.sln
start .\indemind_demos.sln
echo.

cd ..
copy %cd%\..\src\detector\lib\win10-x64\MNN.dll %cd%\output\bin\MNN.dll
copy %cd%\..\lib\win10-x64\indemind.dll %cd%\output\bin\indemind.dll
copy %cd%\..\src\detector\config\blazeface.mnn %cd%\output\bin\blazeface.mnn

::pause
exit
