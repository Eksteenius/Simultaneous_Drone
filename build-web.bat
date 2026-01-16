cd C:\Users\Ethan\Documents\Work\Programming\Other\emscripten
call emsdk_env.bat
set PATH=%PATH%;C:\Users\Ethan\Documents\Work\Programming\Other\ninja
cd C:\Users\Ethan\Documents\Work\Github\Simultaneous_Drone
cmake --build out/build-web
cd out/build-web
python -m http.server 8080