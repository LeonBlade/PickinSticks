CC=g++
CFLAGS=-c -Wall
LDFLAGS=-framework OpenGL -framework GLUT -framework libpng
SOURCES=main.cpp \
		Log.cpp \
		Graphics.cpp \
		Sprite.cpp \
		Map.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=PickinSticks

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
