CC = g++

FILES = main.cpp mandelbrot.cpp palette.cpp timestamp.cpp

FLAGS = -std=c++11

TARGET = mandelbrot

LIBS = -lSDL2 -pthread

$(TARGET):
	$(CC) $(FLAGS) $(FILES) -o $(TARGET) $(LIBS)

all: 
	$(TARGET)

clean: 
	rm $(TARGET)
