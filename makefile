CC = gcc -g
CFLAGS = -Wall -Wextra -pedantic -ansi

HEADERS = deleted.h index.h command.h library.h
EXE     = library
OBJ     = $(EXE).o deleted.o index.o command.o

all: $(EXE)

%.o: %.c $(HEADERS)
	@echo "Compilando $<..."
	$(CC) $(CFLAGS) -c -o $@ $<

$(EXE): $(OBJ)
	@echo "Enlazando $(EXE)..."
	$(CC) -o $(EXE) $(OBJ)
	@echo "Compilación completada con éxito."

clean:
	@echo "Limpiando archivos objeto y binarios..."
	rm -f *.o core $(EXE)

run:
	@echo "Ejecutando programa..."
	./$(EXE) worst_fit test

valgrind:
	@echo "Ejecutando valgrind..."
	valgrind --leak-check=full ./$(EXE) worst_fit test
