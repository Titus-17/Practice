CFLAGS          := -I/usr/local/include/opencv -L/usr/local/lib -Wl,-rpath,/usr/local/lib
OBJECTS         := 
LIBRARIES       := -lopencv_core -lopencv_imgproc -lopencv_highgui

.PHONY: all clean

all: test
test:
	g++ -c rect.cpp
	g++ $(CFLAGS) -c RGB.cpp $(LIBRARIES) 
	g++ $(CFLAGS) -c Func.cpp $(LIBRARIES)
	g++ $(CFLAGS) Scrum.cpp -o ScrumHelper RGB.o Func.o rect.o $(LIBRARIES) 
        
clean:
	rm -f *.o
