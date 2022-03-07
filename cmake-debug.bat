cd _build
call activate_run.bat
cmake -G "Visual Studio 16 2019" -A x64 ..
call deactivate_run.bat
PAUSE