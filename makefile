CC=gcc
CFLAGS=-I.
DEPS = 
OBJ = Emulador.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

Emulador.exe: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)