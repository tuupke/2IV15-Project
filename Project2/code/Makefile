CXX = g++
CXXFLAGS = -g -O2 -Wall -Wno-sign-compare -Iinclude -DHAVE_CONFIG_H -g
OBJS = StableFluids.o ScalarField.o VectorField.o FieldToolbox.o imageio.o Rectangle.o

project2: $(OBJS)
	$(CXX) -o $@ $^ -lGLU -lglut -lpng  -lGL
clean:
	rm $(OBJS) project2
