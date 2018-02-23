# Building
CC = g++
CFLAGS = -lGL -lGLU -lglut -O3 -std=c++11
OUT = out/simulation
IN = src/simulation.cpp

# Formatting
CLANG_STYLE = {BasedOnStyle: google, IndentWidth: 4}


mainmake: $(IN)
	mkdir -p out
	$(CC) $(IN) -o $(OUT) $(CFLAGS)

.PHONY: clean

clean: $(OUT)
	rm  -r out

.PHONY: format

format: $(IN)
	clang-format -i $(IN) -style="$(CLANG_STYLE)"	