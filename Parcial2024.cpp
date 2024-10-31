#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <stack>
#include <unordered_map>

using namespace std;

struct Libro {
    string titulo;
    string autor;
    int anoEdicion;
    string editorial;
    string isbn;
    int paginas;
    bool disponible;
    Libro* siguiente;
};

struct Lector {
    string nombre;
    int dni;
    string libroSolicitado;
    Lector* siguiente;
};

Libro* listaLibros = nullptr;
queue<Lector*> colaSolicitudes;
stack<string> historial;
unordered_map<string, string> librosSolicitados;

void agregarLibro(Libro*& lista, const string& titulo, const string& autor, int ano, const string& editorial, const string& isbn, int paginas, bool disponible);
void cargarLibros(Libro*& lista);
void guardarLibros(Libro* lista);
void ordenarLibrosPorTitulo(Libro*& lista);
void mostrarHistorial();
void vaciarBiblioteca(Libro*& lista);
Libro* buscarLibroPorTitulo(Libro* lista, const string& titulo);
void solicitarLibro(queue<Lector*>& cola, const string& nombre, int dni, const string& libroSolicitado);
void devolverLibro(queue<Lector*>& cola, const string& libroDevuelto);
void guardarSolicitud(Lector* lector); // Prototipo de la función

void agregarLibro(Libro*& lista, const string& titulo, const string& autor, int ano, const string& editorial, const string& isbn, int paginas, bool disponible) {
    Libro* nuevoLibro = new Libro{titulo, autor, ano, editorial, isbn, paginas, disponible, nullptr};
    if (!lista) {
        lista = nuevoLibro;
    } else {
        Libro* actual = lista;
        while (actual->siguiente) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevoLibro;
    }
    historial.push("Libro agregado: " + titulo);
}

void cargarLibros(Libro*& lista) {
    ifstream archivo("biblioteca.txt");
    if (archivo.is_open()) {
        string titulo, autor, editorial, isbn;
        int ano, paginas;
        while (getline(archivo, titulo, ',') &&
               getline(archivo, autor, ',') &&
               archivo >> ano &&
               archivo.ignore() &&
               getline(archivo, editorial, ',') &&
               getline(archivo, isbn, ',') &&
               archivo >> paginas &&
               archivo.ignore()) {
            agregarLibro(lista, titulo, autor, ano, editorial, isbn, paginas, true);
        }
        archivo.close();
        historial.push("Datos de libros cargados");
    } else {
        cout << "No se pudo abrir el archivo para cargar." << endl;
    }
}

void guardarLibros(Libro* lista) {
    ofstream archivo("biblioteca.txt");
    if (archivo.is_open()) {
        for (Libro* actual = lista; actual != nullptr; actual = actual->siguiente) {
            if (actual->disponible) {
                archivo << actual->titulo << "," << actual->autor << "," << actual->anoEdicion << ","
                        << actual->editorial << "," << actual->isbn << "," << actual->paginas << endl;
            }
        }
        archivo.close();
    } else {
        cout << "No se pudo abrir el archivo para guardar." << endl;
    }
}

void ordenarLibrosPorTitulo(Libro*& lista) {
    bool ordenado = false;
    while (!ordenado) {
        ordenado = true;
        Libro* actual = lista;
        Libro* prev = nullptr;
        while (actual && actual->siguiente) {
            if (actual->titulo > actual->siguiente->titulo) {
                if (prev) {
                    prev->siguiente = actual->siguiente;
                } else {
                    lista = actual->siguiente;
                }
                Libro* temp = actual->siguiente->siguiente;
                actual->siguiente->siguiente = actual;
                actual->siguiente = temp;
                ordenado = false;
                continue;
            }
            prev = actual;
            actual = actual->siguiente;
        }
    }
    historial.push("Libros ordenados por título");
}

void mostrarHistorial() {
    cout << "Historial de operaciones:\n";
    stack<string> aux = historial;
    while (!aux.empty()) {
        cout << aux.top() << endl;
        aux.pop();
    }
}

void vaciarBiblioteca(Libro*& lista) {
    while (lista) {
        Libro* temp = lista;
        lista = lista->siguiente;
        delete temp;
    }
    ofstream archivo("biblioteca.txt", ios::trunc);
    archivo.close();
    historial.push("Biblioteca vaciada");
    cout << "La biblioteca ha sido vaciada." << endl;
}

void vaciarSolicitudes() {
    ofstream archivo("solicitudes.txt", ios::trunc); // Abre el archivo y elimina el contenido existente
    if (archivo.is_open()) {
        cout << "El archivo de solicitudes ha sido vaciado." << endl;
        archivo.close();
    } else {
        cout << "No se pudo abrir el archivo para vaciar." << endl;
    }
}


Libro* buscarLibroPorTitulo(Libro* lista, const string& titulo) {
    Libro* actual = lista;
    while (actual) {
        if (actual->titulo == titulo) {
            return actual;
        }
        actual = actual->siguiente;
    }
    return nullptr;
}

void solicitarLibro(queue<Lector*>& cola, const string& nombre, int dni, const string& libroSolicitado) {
    Libro* libro = buscarLibroPorTitulo(listaLibros, libroSolicitado);
    if (!libro || !libro->disponible) {
        cout << "No encontramos el libro que solicitaste o ya está solicitado." << endl;
        return;
    }
    libro->disponible = false;
    Lector* nuevoLector = new Lector{nombre, dni, libroSolicitado, nullptr};
    cola.push(nuevoLector);
    historial.push("Solicitud libro: " + nombre + " para " + libroSolicitado);
    guardarSolicitud(nuevoLector);  // Llama a guardar en el archivo de solicitudes
    guardarLibros(listaLibros);
}

void devolverLibro(queue<Lector*>& cola, const string& libroDevuelto) {
    Libro* libro = buscarLibroPorTitulo(listaLibros, libroDevuelto);
    if (libro && !libro->disponible) {
        libro->disponible = true;
        cout << "Libro '" << libroDevuelto << "' ha sido devuelto y está disponible para ser solicitado nuevamente." << endl;
        guardarLibros(listaLibros);
    } else {
        cout << "Este libro no estaba marcado como solicitado o ya fue devuelto." << endl;
    }
}

void guardarSolicitud(Lector* lector) {
    ofstream archivo("solicitudes.txt", ios::app); // Abre el archivo en modo de agregar
    if (archivo.is_open()) {
        archivo << lector->nombre << "," << lector->dni << "," << lector->libroSolicitado << "\n";
        archivo.close();
    } else {
        cout << "No se pudo abrir el archivo para guardar solicitudes." << endl;
    }
}

int main() {
    int opcion;
    cargarLibros(listaLibros); // Cargar datos desde archivo al iniciar

    do {
        cout << "\nSistema de Gestión de Biblioteca\n";
        cout << "1. Agregar libro\n";
        cout << "2. Ordenar libros por título\n";
        cout << "3. Buscar libro por título\n";
        cout << "4. Solicitar libro\n";
        cout << "5. Devolver libro\n";
        cout << "6. Guardar y salir\n";
        cout << "7. Ver historial de operaciones\n";
        cout << "8. Vaciar biblioteca\n";
        cout << "9. Vaciar archivo de solicitudes\n";
        cout << "0. Salir\n";
        cout << "Opción: ";
        cin >> opcion;
        cin.ignore();

        switch (opcion) {
            case 1: {
                string titulo, autor, editorial, isbn;
                int ano, paginas;
                cout << "Título: "; getline(cin, titulo);
                cout << "Autor: "; getline(cin, autor);

                // Validación de entrada del año de edición
                while (true) {
                    cout << "Año de edición: ";
                    if (!(cin >> ano)) {
                        cin.clear(); // Limpia el estado de error de cin
                        cin.ignore(1000, '\n'); // Ignora hasta 1000 caracteres o hasta el próximo '\n'
                        cout << "Entrada no válida. Ingrese un número para el año de edición.\n";
                    } else {
                        cin.ignore(); // Limpia cualquier sobrante en el buffer
                        break;
                    }
                }

                cout << "Editorial: "; getline(cin, editorial);
                cout << "ISBN: "; getline(cin, isbn);
                cout << "Páginas: "; cin >> paginas; cin.ignore();
                agregarLibro(listaLibros, titulo, autor, ano, editorial, isbn, paginas, true);
                guardarLibros(listaLibros);
                break;
            }
            case 2:
                ordenarLibrosPorTitulo(listaLibros);
                cout << "Libros ordenados por título.\n";
                break;
            case 3: {
                string titulo;
                cout << "Ingrese el título del libro a buscar: "; getline(cin, titulo);
                Libro* encontrado = buscarLibroPorTitulo(listaLibros, titulo);
                if (encontrado) {
                    cout << "Libro encontrado: " << encontrado->titulo << " de " << encontrado->autor << endl;
                } else {
                    cout << "No se encontró el libro o no está disponible.\n";
                }
                break;
            }
            case 4: {
                string nombre, titulo;
                int dni;
                cout << "Nombre del lector: "; getline(cin, nombre);
                cout << "DNI del lector: "; cin >> dni; cin.ignore();
                cout << "Libro solicitado: "; getline(cin, titulo);
                solicitarLibro(colaSolicitudes, nombre, dni, titulo);
                break;
            }
            case 5: {
                string libroDevuelto;
                cout << "Libro devuelto: "; getline(cin, libroDevuelto);
                devolverLibro(colaSolicitudes, libroDevuelto);
                break;
            }
            case 6:
                guardarLibros(listaLibros);
                cout << "Datos guardados. Saliendo...\n";
                break;
            case 7:
                mostrarHistorial();
                break;
            case 8:
                vaciarBiblioteca(listaLibros);
                break;
            case 9:
                 vaciarSolicitudes();
                break;

            case 0:
                cout << "Saliendo...\n";
                break;
            default:
                cout << "Opción no válida.\n";
        }
    } while (opcion != 0);

    return 0;
}
