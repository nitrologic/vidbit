mkdir bin
pushd bin
cmake -G Ninja ..
ninja 
rem reset.exe
rem echo.exe
rem rpc.exe
rem win32tool.exe
tx "{\"jsonrpc\":\"2.0\",\"method\":\"rtc.set\",\"params\":{\"time\":347155200},\"id\":1}"
popd
