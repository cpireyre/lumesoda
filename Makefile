LDFLAGS := -I/opt/homebrew/Cellar/sfml/3.0.2/include
LDFLAGS += -L/opt/homebrew/Cellar/sfml/3.0.2/lib
LDFLAGS += -lsfml-graphics -lsfml-system -lsfml-window

CFLAGS := -std=c++17 -g3 -fsanitize=address

all: main.cpp
	clang++ $(LDFLAGS) $(CFLAGS) main.cpp
	./a.out
