@echo on
CALL emcmake cmake -S . -B out -DBUILD_WEB=ON -DCMAKE_BUILD_TYPE=Release
CALL cmake --build out