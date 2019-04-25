CC     = g++
UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
	FLAGS = -framework OpenGL -framework GLUT
else
	FLAGS = -lGL -lGLU -lglut
endif

gl: main.o RgbImage.o 
	${CC} main.o RgbImage.o -o gl ${FLAGS} -Wno-deprecated

main.o: main.cpp
	${CC} -c main.cpp -Wno-deprecated

RgbImage.o: RgbImage.cpp
	${CC} -c RgbImage.cpp -Wno-deprecated
