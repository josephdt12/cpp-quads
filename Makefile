cpp-quads.out: cpp-quads.cpp
	g++ -std=c++14 cpp-quads.cpp -o cpp-quads.out `pkg-config --cflags --libs opencv`
