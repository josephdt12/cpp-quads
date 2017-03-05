avg.out: averages.cpp
	g++ -std=c++14 averages.cpp -o avg.out `pkg-config --cflags --libs opencv`
