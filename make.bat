SET EMSDKPATH=C:\emsdk-main\emsdk-main

CALL %EMSDKPATH%\emsdk_env.bat

emcc -O1 main.cpp font.cpp scene.cpp primitives.cpp --std=c++14 -s WASM=0 -o zombie.html -fno-exceptions  --embed-file conf

