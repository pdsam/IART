CC= g++
LIBS = 
OBJ = main.o
CFLAGS = -std=c++17 -O2
all: create-dir iart 

create-dir:
	mkdir -p bin
%.o: %.c
			$(CC) -c -o $@ $< $(LIBS) $(CFLAGS)

iart: $(OBJ) $(AOBJ)
			$(CC) $^ $(CFLAGS) $(LIBS) -o bin/$@ 

clean:
	rm -rf $(OBJ)
	rm -rf bin
