mkdir bin
pushd bin
cmake -G Ninja ..
ninja 
rem reset.exe
rem echo.exe
rpc.exe
popd
