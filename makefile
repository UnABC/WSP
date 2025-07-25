COMPILER = g++
OPTIMIZE = -O3
CXXFLAGS = -std=c++23
TARGET   = ./bin/wsp
LDLIBS = -lmingw32 -lSDL3 -lopengl32 -lfreetype -lglew32 -lSDL3_mixer
SRCS = $(wildcard *.cpp)
OBJDIR = ./obj
SOURCES  = $(wildcard src/*.cpp)
OBJECTS  = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(COMPILER) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

$(OBJDIR)/%.o: src/%.cpp 
	$(COMPILER) $(CXXFLAGS) -o $@ -c $<

clean:
	- del obj\*.o
