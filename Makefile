relayExecuteable : main.o
	g++ main.o -o relay -lboost_system -pthread

main.o : main.cpp
	g++ -c main.cpp