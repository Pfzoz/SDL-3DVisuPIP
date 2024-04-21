TARGET=main.x
TEMP_TARGET=temp_main.x

compile: src/main.cc
	g++ src/main.cc -pedantic -w -lSDL2 -lSDL2_image -lSDL2_ttf -o $(TARGET)

run: src/main.cc
	g++ src/main.cc -w -lSDL2 -lSDL2_image -lSDL2_ttf -o $(TEMP_TARGET) && ./$(TEMP_TARGET) && rm ./$(TEMP_TARGET)