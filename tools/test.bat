mkdir bin
pushd bin
cmake -G Ninja ..
ninja 
rem reset.exe
rem echo.exe
rem rpc.exe
win32tool.exe
popd
