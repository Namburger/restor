CXX=g++
INCLUDE=-Iinclude
CXXFLAGS=-std=c++17 -Wall -Wextra -pthread

all: client

# Execute install_dependencies.sh and then build example client.cc 
install:
	echo '-------------installing dependencies------------'
	bash install_dependencies.sh
uninstall:
	rm -rf include

client: main.cc Makefile
	$(CXX) $(INCLUDE) $(CXXFLAGS) -o restor_client main.cc
clean:
	rm restor_client
