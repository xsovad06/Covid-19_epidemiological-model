CC=g++
FLAGS=-g -pedantic -std=c++17
FILE=automat

all:
	$(CC) $(FLAGS) $(FILE).cpp -o $(FILE)

run:
	$(CC) $(FLAGS) $(FILE).cpp -o $(FILE)
	./$(FILE) 200 40 8 6
	-rm $(FILE)

clean:
	-rm $(FILE)
