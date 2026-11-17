all:
	clang++ snake.cpp -o snake -lraylib -lm -ldl -lpthread -lGL -lX11

clean:
	rm snake
