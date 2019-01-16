git clone https://github.com/gabime/spdlog
cp -r spdlog/include/spdlog include/
sudo apt-get install libsfml-dev

cmake .
make
