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

echo start... "call cmake ."
call cmake ..\.
echo.

echo .\indemind_demos.sln
start .\indemind_demos.sln

::pause
exit
