CC = "g++"
PROJECT = pacman
SRC = ./src/pacman.cpp

LIBS = `pkg-config opencv4 --cflags --libs` -lpthread -fopenmp

$(PROJECT) : $(SRC)
	$(CC) -g $(SRC) -o $(PROJECT) $(LIBS)

run:
	./$(PROJECT)

clean:
	rm pacman 
