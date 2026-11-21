all: build
	clang++ src/snake.cpp -o build/snake -lraylib -lm -ldl -lpthread -lGL -lX11

clean:
	rm build/snake
	rmdir build

build:
	mkdir -p build
