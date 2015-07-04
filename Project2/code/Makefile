CXX = g++
CXXFLAGS = -g -O2 -Wall -Wno-sign-compare -Iinclude -DHAVE_CONFIG_H -g
OBJS = StableFluids.o ScalarField.o VectorField.o FieldToolbox.o imageio.o 
PART_OBJS = particles/Solver.o particles/Particle.o particles/RodConstraint.o particles/SpringForce.o particles/CircularWireConstraint.o particles/Gravity.o particles/AngularForce.o particles/MouseForce.o particles/ConstraintSolver.o #particles/TinkerToy.o

project2: $(OBJS) $(PART_OBJS)
	$(CXX) -O2 -o $@ $^ -lGLU -lglut -lpng  -lGL -ffast-math
clean:
	rm $(OBJS) $(PART_OBJS) project2
