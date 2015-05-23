CXX = g++
CXXFLAGS = -O2 -Wall -Wno-sign-compare -Iinclude -DHAVE_CONFIG_H 
OBJS = Solver.o Particle.o TinkerToy.o RodConstraint.o SpringForce.o CircularWireConstraint.o imageio.o Gravity.o AngularForce.o

project1: $(OBJS)
	$(CXX) -o $@ $^ -lglut -lGLU -lpng -lGL
clean:
	rm $(OBJS) project1
