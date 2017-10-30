COMPILER = mpicxx
PROGRAM = highres
CXXFLAGS = -O2 -w -I/opt/local/include/openmpi-mp

# Search for code files in local and common folders
VPATH = ../Common
SRC = $(wildcard ../Common/*.cpp)
SRC += $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SRC))

# Link libraries
LDFLAGS = -lGL -lglut -lz
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
LDFLAGS = -framework OpenGL -framework GLUT -lz
endif

all : $(PROGRAM)

$(PROGRAM) : $(OBJS)
	$(COMPILER) -o $(PROGRAM) $(OBJS) $(LDFLAGS)

%.o : %.c
	$(COMPILER) -c $<

clean :
	rm *.o
	rm ../Common/*.o
