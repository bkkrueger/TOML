CPP = g++ -std=c++11

test_driver : test_driver.o config.o
	$(CPP) test_driver.o config.o -o test_driver

test_driver.o : test_driver.cpp config.h
	$(CPP) -c test_driver.cpp -o test_driver.o

config.o : config.cpp config.h
	$(CPP) -c config.cpp -o config.o
