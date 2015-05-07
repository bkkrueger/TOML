CPP = g++ -std=c++11

all : test_driver particle

config.o : config.cpp config.h
	$(CPP) -c config.cpp -o config.o

test_driver : test_driver.o config.o
	$(CPP) test_driver.o config.o -o test_driver

test_driver.o : test_driver.cpp config.h
	$(CPP) -c test_driver.cpp -o test_driver.o

particle : particle.o config.o
	$(CPP) particle.o config.o -o particle

particle.o : particle.cpp config.h
	$(CPP) -c particle.cpp -o particle.o

clean :
	rm -f config.o test_driver.o particle.o

realclean : clean
	rm -f test_driver particle
