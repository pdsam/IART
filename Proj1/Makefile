CC= g++
LIBS = 
OBJ = main.o
CFLAGS = -std=c++17
DEBUG_FLAGS = -O0 -g
RELEASE_FLAGS = -O2 -DNDEBUG
all: create-dir debug 

create-dir:
	mkdir -p bin
%.o: %.c
			$(CC) -c -o $@ $< $(LIBS) $(CFLAGS)

release: $(OBJ) 
			$(CC) $^ $(CFLAGS) $(RELEASE_FLAGS) $(LIBS) -o bin/$@ 

debug: $(OBJ)
			$(CC) $^ $(CFLAGS) $(DEBUG_FLAGS) $(LIBS) -o bin/$@ 

clean:
	rm -rf $(OBJ)
	rm -rf bin
