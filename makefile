COMPILER = g++
OPTIMIZE = -O3
CXXFLAGS = -std=c++23
TARGET   = ./bin/wsp
LDLIBS = -lmingw32 -lSDL3 -lopengl32 -lSDL3_ttf
SRCS = main.cpp lexer.cpp parser.cpp evaluator.cpp Var.cpp graphic.cpp
OBJDIR = ./obj
SOURCES  = $(wildcard *.cpp)
OBJECTS  = $(addprefix $(OBJDIR)/, $(SOURCES:.cpp=.o))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(COMPILER) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

$(OBJDIR)/%.o: %.cpp 
	$(COMPILER) $(CXXFLAGS) -o $@ -c $<

clean:
	- del obj\*.o
