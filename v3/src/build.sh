rm -rf core

ulimit -c unlimited

sh clear.sh

cd base
make
cd ..
make

./HttpServer 4

