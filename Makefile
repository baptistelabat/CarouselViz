OBJECTS = UDP.o CarouselViz.o
MAIN_BINARIES = $(basename $(wildcard *Main.cpp))
TEST_BINARIES = $(basename $(wildcard *Test.cpp))
HEADERS = $(wildcard *.h)
CXX = g++ -O3 -Wall -lGL -lglut -lGLU
#CXX = g++ -lGL -lglut -lGLU -Wall -o COMET SystemMain.cpp imageloader.cpp

all: compile checkstyle test

#compile: $(MAIN_BINARIES) $(TEST_BINARIES)
compile: 
	g++ CarouselVizMain.cpp CarouselViz.cpp UDP.cpp glm.c -lGL -lGLU -lglut -o CarouselVizMain

%.o: %.cpp $(HEADERS)
	$(CXX) -c $<

%Main: %Main.o $(OBJECTS)
	$(CXX) -o $@ $^

%Test: %Test.o $(OBJECTS)
	$(CXX) -o $@ $^ -lgtest -lgtest_main -lpthread

checkstyle:
	python ../cpplint.py *.h *.cpp

test: $(TEST_BINARIES)
	./$(TEST_BINARIES)

clean:
	rm -f $(MAIN_BINARIES)
	rm -f $(TEST_BINARIES)
	rm -f *.o
	rm -f *~
	rm -f *.txt
