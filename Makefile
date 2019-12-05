relayExecuteable : main.o
	g++ main.o -o relay -lboost_system

main.o : main.cpp
	g++ -c main.cpp