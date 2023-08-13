HEADERS = include/
SRC = src/
FLAGS = -Wall -ansi -pedantic
PROGRAM = client

$(PROGRAM): $(SRC)/* $(HEADERS)/*
	gcc $(SRC)/* -I $(HEADERS) -o $(PROGRAM) $(FLAGS)

clean:
	rm $(PROGRAM)