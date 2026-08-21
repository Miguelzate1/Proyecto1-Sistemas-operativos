# Proyecto de Sistemas Operativos
## Implementación de un Shell para xv6

### Integrantes

-Miguel Angel ALzate Chavarria
-Samuel Quintero

### Curso

Sistemas Operativos

### Semestre

2026-2

---

# Descripción

Este proyecto implementa un shell (`user/sh.c`) para el sistema operativo xv6. El programa
lee comandos desde la entrada estándar, los interpreta y ejecuta los programas existentes
en xv6 mediante la creación de nuevos procesos con `fork()` y `exec()`.

La implementación soporta:

- Ejecución de comandos simples.
- Ejecución de comandos con argumentos.
- Redirección de entrada (`<`).
- Redirección de salida (`>`).
- Tuberías simples y múltiples (`|`).
- Comando interno `exit` para finalizar el shell.

La solución fue desarrollada en lenguaje C y se integra al proceso de compilación
estándar de xv6, ejecutado sobre QEMU.

---

# Estructura de archivos

```text
Makefile
Readme
user/
├── sh.c
├── parser.c
├── parser.h
├── commands.c
└── commands.h
```

## Descripción de los archivos

### Makefile

Es el Makefile original de xv6, con dos cambios puntuales para integrar el shell:

- Se agregó una regla específica para `$U/_sh` (en vez de dejar que use la regla genérica
  `_%: %.o`, que solo enlaza un `.o`). Esta regla nueva compila y enlaza juntos
  `sh.o`, `parser.o` y `commands.o`, porque el shell quedó dividido en varios archivos
  fuente:
  ```make
  $U/_sh: $U/sh.o $U/parser.o $U/commands.o $(ULIB) $U/user.ld
  	$(LD) $(LDFLAGS) -T $U/user.ld -o $U/_sh $U/sh.o $U/parser.o $U/commands.o $(ULIB)
  ```
- Se agregó `$U/_sh` a la lista `UPROGS`, para que el binario del shell quede incluido en
  la imagen del sistema de archivos (`fs.img`) que arranca xv6.

El resto del Makefile no fue modificado.

### sh.c

Archivo principal del shell. Contiene el ciclo principal (`main`) que lee cada línea
ingresada por el usuario (`getcmd`), detecta el comando interno `exit`, y por cada línea
válida crea un proceso hijo con `fork()`. El proceso hijo interpreta la línea con
`parse_command()` y la ejecuta con `ejecutar_comando_complejo()`; el proceso padre espera
la finalización del hijo con `wait()` antes de volver a leer la siguiente línea.

### parser.c

Contiene `parse_command()`, la función encargada de recorrer la línea de texto y separarla
en tokens (argumentos) usando espacios y tabulaciones como delimitadores. Cada token queda
guardado en `cmd->argv`, y el arreglo termina en `NULL` porque así lo requiere `exec()`.
No interpreta el significado de los tokens (no distingue `<`, `>` o `|`); solo los separa.

### parser.h

Define la estructura `Command` (arreglo `argv` de hasta `MAXARGS` punteros y un contador
`argc`) y declara el prototipo de `parse_command()`.

### commands.c

Implementa la lógica de ejecución:

- `ejecutar_comando_complejo()`: recorre los argumentos buscando el operador `|`. Si lo
  encuentra, crea un `pipe()` y dos procesos hijos (uno para el comando a la izquierda del
  `|`, otro para el resto a la derecha), conectando la salida del primero con la entrada
  del segundo mediante `dup()`. Como la mitad derecha se vuelve a pasar a la misma función
  de forma recursiva, esto permite encadenar más de una tubería (por ejemplo
  `cat archivo.txt | grep error | wc`). Si no hay `|`, revisa si hay `>` o `<` y delega en
  las funciones de redirección; si no hay ninguno de los dos, ejecuta el comando
  directamente con `exec()`.
- `ejecutar_redireccion()`: abre (o crea) el archivo de salida, reemplaza el descriptor 1
  (stdout) por ese archivo usando `dup()`, y ejecuta el comando con `exec()`.
- `ejecutar_redireccion_entrada()`: abre el archivo de entrada en modo lectura, reemplaza
  el descriptor 0 (stdin) por ese archivo, y ejecuta el comando con `exec()`.

### commands.h

Declara los prototipos de las tres funciones implementadas en `commands.c`.

---

# Integración con xv6

### Paso 1. Obtener xv6

```bash
git clone https://github.com/mit-pdos/xv6-riscv.git
```

## Paso 2. Copiar archivos

Copiar `sh.c`, `parser.c`, `parser.h`, `commands.c` y `commands.h` dentro del directorio
`user/` de xv6, reemplazando el `sh.c` original. Copiar también el `Makefile` de este
repositorio, reemplazando el `Makefile` original de xv6.

## Paso 3. Makefile

Este repositorio incluye el `Makefile` ya modificado (no hace falta editarlo a mano):
tiene una regla propia para enlazar `$U/_sh` a partir de `sh.o`, `parser.o` y `commands.o`
juntos, y `$U/_sh` fue agregado a la lista `UPROGS` para que el binario quede incluido en
la imagen de disco de xv6. Los archivos `parser.o` y `commands.o` se compilan solos gracias
a las reglas implícitas de `make` para archivos `.c`, que ya vienen definidas en el
Makefile original.

## Paso 4. Compilar xv6

```bash
make qemu
```

## Paso 5. Ejecutar el shell

```bash
sh
```

---
# Casos Prueba
## --- PREPARACIÓN DE ARCHIVOS DE PRUEBA ---

echo "texto de prueba" > archivo.txt

echo "este archivo tiene un error critico" >> archivo.txt

echo "error en el sistema operativo" >> datos.txt

### --- 1. Ejecución simple ---
ls

   Salida: Muestra la lista de archivos (sh, archivo.txt, datos.txt, etc.)

echo hola

   Salida: hola


### --- 2. Argumentos ---
echo hola mundo

   Salida: hola mundo

grep texto archivo.txt

 Salida: texto de prueba


### --- 3. Redirección de entrada (<) ---
cat < datos.txt

 Salida: error en el sistema operativo


### --- 4. Redirección de salida (>) ---
echo hola > salida.txt
cat < salida.txt

Salida: hola


### --- 5. Tuberías simples (|) ---
echo hola | wc

 Salida: Conteo de líneas, palabras y bytes (ej: 1 1 5)


### --- 6. Tuberías múltiples (| |) ---
cat archivo.txt | grep error | wc

 Salida: Conteo de las líneas que contienen "error" (ej: 1 6 36)

ls | grep sh | sort | wc

 Salida: Conteo del comando filtrado


### --- 7. Comando exit ---
exit

 Salida: Cierra el shell de forma controlada y muestra: init: starting sh.


## Decisiones de diseño

- Separación entre el análisis de la línea de comandos (`parser.c`) y su ejecución
  (`commands.c` y `sh.c`).
- El ciclo principal crea un proceso hijo por cada línea ingresada; el hijo interpreta y
  ejecuta el comando, mientras el padre espera con `wait()`. Esto evita que un error en un
  comando termine el shell.
- El comando `exit` se resuelve en el proceso padre, antes de crear el hijo, para cerrar el
  shell de forma controlada sin procesos huérfanos.
- Las tuberías se resuelven de forma recursiva sobre el mismo arreglo de argumentos: se
  localiza el primer `|`, se parte el arreglo en dos mitades y cada mitad se ejecuta en un
  proceso distinto conectado por un `pipe()`. Como la mitad derecha puede volver a contener
  otro `|`, la recursión permite soportar tuberías múltiples sin límite fijo.
- Las redirecciones (`<` y `>`) se implementan cerrando el descriptor estándar
  correspondiente (0 o 1) y duplicando en su lugar el descriptor del archivo abierto, antes
  de llamar a `exec()`.

---

# Limitaciones conocidas

- No soporta comillas simples ni dobles.
- No soporta variables de entorno ni sustitución de variables.
- No soporta expansión de comodines.
- No soporta ejecución en segundo plano ni control de trabajos.
- No permite combinar redirección de entrada y salida en el mismo comando.
- No implementa `cd`, `history`, `jobs`, `fg`, `bg` ni `kill` (fuera del alcance del
  proyecto según el enunciado).

---

# Declaración de uso de IA

Durante el desarrollo del proyecto se utilizaron herramientas de inteligencia artificial
generativa como apoyo para comprensión de conceptos, revisión de código y generación de
ejemplos. Los integrantes comprenden completamente las soluciones utilizadas y asumen
plena responsabilidad académica sobre el contenido entregado.
