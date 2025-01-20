mkdir -p build
cd build
rm -f monitor_exe
clear

g++ -std=c++23 -Ofast -o monitor_exe ../monitor.cpp ../index.cpp ../util.cpp

./monitor_exe

cd -
rm -rf build
echo "Build and Run is Done!"