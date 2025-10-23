SFML_DIR := SFML-2.6.0
SFML_INCLUDE := $(SFML_DIR)/include
SFML_LIB := $(SFML_DIR)/build/lib

CFLAGS := -std=c++17 -g3 -fsanitize=address -DSFML_STATIC
LDFLAGS := -I$(SFML_INCLUDE)

LIBS := -L$(SFML_LIB) -lsfml-graphics-s -lsfml-window-s -lsfml-system-s
LIBS += -framework OpenGL -framework Foundation -framework AppKit -framework IOKit -framework Carbon

all: main.cpp
	clang++ $(CFLAGS) $(LDFLAGS) main.cpp $(LIBS)
	./a.out
