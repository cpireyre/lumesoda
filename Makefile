SFML_DIR := ./SFML-2.6.2/
SFML_INCLUDE := $(SFML_DIR)/include
SFML_LIB := $(SFML_DIR)/build/lib

CFLAGS := -std=c++17 -I$(SFML_INCLUDE)
LDFLAGS := -L$(SFML_LIB) -Wl,-rpath,$(SFML_LIB)

LIBS := -lsfml-graphics -lsfml-window -lsfml-system
LIBS += -framework OpenGL -framework Foundation -framework AppKit -framework IOKit -framework Carbon
LIBS += $(shell pkg-config --libs freetype2)

all: main.cpp
	clang++ $(CFLAGS) $(LDFLAGS) main.cpp $(LIBS)
	./a.out
