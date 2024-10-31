import csv
from collections import deque

class Libro:
    def __init__(self, titulo, autor, ano_edicion, editorial, isbn, paginas, disponible=True):
        self.titulo = titulo
        self.autor = autor
        self.ano_edicion = ano_edicion
        self.editorial = editorial
        self.isbn = isbn
        self.paginas = paginas
        self.disponible = disponible

class Lector:
    def __init__(self, nombre, dni, libro_solicitado):
        self.nombre = nombre
        self.dni = dni
        self.libro_solicitado = libro_solicitado

# Variables globales
lista_libros = []
cola_solicitudes = deque()
historial = []

# Funciones
def agregar_libro(titulo, autor, ano, editorial, isbn, paginas):
    nuevo_libro = Libro(titulo, autor, ano, editorial, isbn, paginas)
    lista_libros.append(nuevo_libro)
    historial.append(f"Libro agregado: {titulo}")
    guardar_libros()

def cargar_libros():
    try:
        with open("bibliotecapython.csv", newline='', encoding="utf-8") as archivo:
            lector_csv = csv.reader(archivo)
            for fila in lector_csv:
                titulo, autor, ano, editorial, isbn, paginas = fila
                agregar_libro(titulo, autor, int(ano), editorial, isbn, int(paginas))
        print("Datos de libros cargados")
    except FileNotFoundError:
        print("No se pudo abrir el archivo para cargar.")

def guardar_libros():
    with open("bibliotecapython.csv", mode="w", newline='', encoding="utf-8") as archivo:
        escritor_csv = csv.writer(archivo)
        for libro in lista_libros:
            if libro.disponible:
                escritor_csv.writerow([libro.titulo, libro.autor, libro.ano_edicion, 
                                       libro.editorial, libro.isbn, libro.paginas])

def ordenar_libros_por_titulo():
    lista_libros.sort(key=lambda libro: libro.titulo)
    historial.append("Libros ordenados por titulo")
    guardar_libros()
    print("Libros ordenados por título y guardados en bibliotecapython.csv.")

def mostrar_historial():
    print("Historial de operaciones:")
    for operacion in historial:
        print(operacion)

def buscar_libro_por_titulo(titulo):
    for libro in lista_libros:
        if libro.titulo == titulo:
            return libro
    return None

def solicitar_libro(nombre, dni, titulo_libro):
    libro = buscar_libro_por_titulo(titulo_libro)
    if not libro or not libro.disponible:
        print("No encontramos el libro que solicitaste o ya está solicitado.")
        return

    libro.disponible = False
    nuevo_lector = Lector(nombre, dni, titulo_libro)
    cola_solicitudes.append(nuevo_lector)
    historial.append(f"Solicitud libro: {nombre} para {titulo_libro}")
    guardar_solicitud(nuevo_lector)
    guardar_libros()

def devolver_libro(titulo_libro):
    libro = buscar_libro_por_titulo(titulo_libro)
    if libro and not libro.disponible:
        libro.disponible = True
        print(f"Libro '{titulo_libro}' ha sido devuelto y está disponible para ser solicitado nuevamente.")
        guardar_libros()
    else:
        print("Este libro no estaba marcado como solicitado o ya fue devuelto.")

def vaciar_biblioteca():
    lista_libros.clear()
    with open("bibliotecapython.csv", "w", encoding="utf-8") as archivo:
        pass  # Esto vacía el contenido del archivo
    historial.append("Biblioteca vaciada")
    print("La biblioteca ha sido vaciada.")

def vaciar_solicitudes():
    cola_solicitudes.clear()
    with open("solicitudpython.txt", "w", encoding="utf-8") as archivo:
        pass  # Esto vacía el contenido del archivo
    print("El archivo de solicitudes ha sido vaciado.")

def guardar_solicitud(lector):
    with open("solicitudpython.txt", "a", encoding="utf-8") as archivo:
        archivo.write(f"{lector.nombre},{lector.dni},{lector.libro_solicitado}\n")

# Menú principal
def main():
    cargar_libros()

    while True:
        print("\nSistema de Gestión de Biblioteca")
        print("1. Agregar libro")
        print("2. Ordenar libros por título")
        print("3. Buscar libro por título")
        print("4. Solicitar libro")
        print("5. Devolver libro")
        print("6. Guardar y salir")
        print("7. Ver historial de operaciones")
        print("8. Vaciar biblioteca")
        print("9. Vaciar archivo de solicitudes")
        print("0. Salir")
        opcion = input("Opción: ")

        if opcion == "1":
            titulo = input("Título: ")
            autor = input("Autor: ")

            # Validación para el año de edición
            while True:
                try:
                    ano = int(input("Año de edición: "))
                    break
                except ValueError:
                    print("Ingrese un número para el año de edición.")

            editorial = input("Editorial: ")
            isbn = input("ISBN: ")

            # Validación para la cantidad de páginas
            while True:
                try:
                    paginas = int(input("Páginas: "))
                    break
                except ValueError:
                    print("Ingrese un número para la cantidad de páginas.")

            agregar_libro(titulo, autor, ano, editorial, isbn, paginas)

        elif opcion == "2":
            ordenar_libros_por_titulo()

        elif opcion == "3":
            titulo = input("Ingrese el título del libro a buscar: ")
            libro = buscar_libro_por_titulo(titulo)
            if libro:
                print(f"Libro encontrado: {libro.titulo} de {libro.autor}")
            else:
                print("No se encontró el libro o no está disponible.")

        elif opcion == "4":
            nombre = input("Nombre del lector: ")
            dni = input("DNI del lector: ")
            titulo_libro = input("Libro solicitado: ")
            solicitar_libro(nombre, dni, titulo_libro)

        elif opcion == "5":
            titulo_libro = input("Libro devuelto: ")
            devolver_libro(titulo_libro)

        elif opcion == "6":
            guardar_libros()
            print("Datos guardados. Saliendo...")
            break

        elif opcion == "7":
            mostrar_historial()

        elif opcion == "8":
            vaciar_biblioteca()

        elif opcion == "9":
            vaciar_solicitudes()

        elif opcion == "0":
            print("Saliendo...")
            break

        else:
            print("Opción no válida.")

if __name__ == "__main__":
    main()




