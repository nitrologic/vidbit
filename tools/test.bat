mkdir bin
pushd bin
cmake -G Ninja ..
ninja 
reset.exe
popd
