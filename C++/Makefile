CPP = g++ -std=c++11

all : test_driver particle

toml.o : toml.cpp toml.h
	$(CPP) -c toml.cpp -o toml.o

test_driver : test_driver.o toml.o
	$(CPP) test_driver.o toml.o -o test_driver

test_driver.o : test_driver.cpp toml.h
	$(CPP) -c test_driver.cpp -o test_driver.o

particle : particle.o toml.o
	$(CPP) particle.o toml.o -o particle

particle.o : particle.cpp toml.h
	$(CPP) -c particle.cpp -o particle.o

clean :
	rm -f toml.o test_driver.o particle.o

realclean : clean
	rm -f test_driver particle
