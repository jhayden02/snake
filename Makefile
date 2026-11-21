CC := clang++ --std=c++17 
LINK_FLAGS := -lraylib -lm -ldl -lpthread -lGL -lX11

all: build
	$(CC) src/snake.cpp -o build/snake $(LINK_FLAGS)

clean:
	rm build/snake
	rmdir build

build:
	mkdir -p build
