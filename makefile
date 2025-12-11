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

best_fit:
	@echo "Ejecutando programa para estrategia best_fit..."
	./$(EXE) best_fit test

first_fit:
	@echo "Ejecutando programa para estrategia first_fit..."
	./$(EXE) first_fit test

worst_fit:
	@echo "Ejecutando programa para estrategia worst_fit..."
	./$(EXE) worst_fit test

best_valgrind:
	@echo "Ejecutando valgrind para estrategia best_fit..."
	valgrind --leak-check=full ./$(EXE) best_fit test

first_valgrind:
	@echo "Ejecutando valgrind para estrategia first_fit..."
	valgrind --leak-check=full ./$(EXE) first_fit test

worst_valgrind:
	@echo "Ejecutando valgrind para estrategia worst_fit..."
	valgrind --leak-check=full ./$(EXE) worst_fit test
