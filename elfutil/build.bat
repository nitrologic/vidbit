rem set error

rem copy ..\..\entheogen\vidbit\bin\vidbot1.elf .

mkdir bin
pushd bin
cmake -G Ninja ..
ninja 

if %errorlevel% neq 0 (
    echo cmake ninja failure errorlevel:%errorlevel%
    exit /b %errorlevel%
)

popd

bin\elfutil.exe vidbot1.elf

