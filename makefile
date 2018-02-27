# Building
CC = g++
CFLAGS = -lGL -lGLU -lglut -O3 -std=c++11
OUT = out/simulation
IN = src/simulation.cpp

# Formatting
CLANG_STYLE = {BasedOnStyle: google, IndentWidth: 4}


mainmake: simulation.o Particle.o System.o
	mkdir -p out
	$(CC) -o $(OUT) Particle.o System.o simulation.o $(CFLAGS) 

simulation.o: src/simulation.cpp
	$(CC) $(CFLAGS) -c src/simulation.cpp

Particle.o: src/Particle.cpp
	$(CC) $(CFLAGS) -c src/Particle.cpp

System.o: src/System.cpp
	$(CC) $(CFLAGS) -c src/System.cpp

test.o: src/test.cpp
	$(CC) $(CFLAGS) -c src/test.cpp

.PHONY: clean

clean:
	rm -r -f out/ *.o a.out 
.PHONY: format

format: $(IN)
	clang-format -i $(IN) -style="$(CLANG_STYLE)"	