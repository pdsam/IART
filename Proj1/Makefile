CC= g++
LIBS = 
OBJ = main.o
CFLAGS = -std=c++17
DEBUG_FLAGS = -O0 -g
RELEASE_FLAGS = -O2 -DNDEBUG
all: debug 

create-dir:
	mkdir -p bin

%.o: %.cpp
			$(CC) $(CFLAGS) -c -o $@ $< $(LIB)

release: CFLAGS += $(RELEASE_FLAGS)
release: $(OBJ) 
			$(CC) $^ $(CFLAGS) $(RELEASE_FLAGS) $(LIBS) -o bin/$@ 

debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(OBJ) 
			$(CC) $^ $(CFLAGS) $(DEBUG_FLAGS) $(LIBS) -o bin/$@ 

clean:
	rm -rf $(OBJ)
