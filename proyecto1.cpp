#include <iostream>    // Para entrada/salida estándar
#include <string>      // Para manejar cadenas de texto
#include <vector>      // Para manejar listas de palabras (diccionario)
#include <fstream>     // Para leer archivos (por ejemplo, diccionarios)
#include <algorithm>   // Para funciones como std::next_permutation
#include <chrono>
#include <thread>
#include <cctype>
#include <atomic>
#ifdef _WIN32
#include <conio.h>
#endif

std::string fuerzaBrutaMultiHilo(const std::string& objetivo, const std::string& alfabeto) {
    int longitud = objetivo.length();
    size_t n = alfabeto.size();
    std::atomic<bool> encontrada(false);
    std::string resultado;
    int num_hilos = std::thread::hardware_concurrency();
    if (num_hilos == 0) num_hilos = 4; // Valor por defecto si no se detecta

    auto inicio = std::chrono::high_resolution_clock::now();

    auto worker = [&](int inicio_idx) {
        std::string intento(longitud, alfabeto[0]);
        std::vector<int> indices(longitud, 0);
        indices[0] = inicio_idx;
        while (!encontrada) {
            // Construir el intento actual
            for (int i = 0; i < longitud; ++i) {
                intento[i] = alfabeto[indices[i]];
            }
            if (intento == objetivo) {
                encontrada = true;
                resultado = intento;
                break;
            }
            // Incrementar los índices (como un contador en base n)
            int pos = longitud - 1;
            while (pos >= 0) {
                indices[pos]++;
                if (indices[pos] < n) {
                    break;
                } else {
                    indices[pos] = 0;
                    pos--;
                }
            }
            if (pos < 0 || encontrada) break; // Probamos todas las combinaciones o ya se encontró
            // Limitar el trabajo de cada hilo a su rama
            if (indices[0] != inicio_idx) break;
        }
    };

    std::vector<std::thread> hilos;
    int max_hilos = std::min(num_hilos, (int)n);
    for (int i = 0; i < max_hilos; ++i) {
        hilos.emplace_back(worker, i);
    }
    for (auto& t : hilos) t.join();

    auto fin = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duracion = fin - inicio;

    if (encontrada) {
        std::cout << "¡Contraseña encontrada!" << std::endl;
    } else {
        std::cout << "No se pudo encontrar la contraseña." << std::endl;
    }
    std::cout << "Tiempo transcurrido: " << duracion.count() << " segundos." << std::endl;
    return resultado;
}

// Fuerza bruta incremental: prueba todas las combinaciones de longitud creciente
std::string fuerzaBrutaIncremental(const std::string& objetivo, const std::string& alfabeto) {
    int longitud_max = objetivo.length();
    size_t n = alfabeto.size();
    std::string intento;
    auto inicio = std::chrono::high_resolution_clock::now();
    for (int longitud = 1; longitud <= longitud_max; ++longitud) {
        std::vector<int> indices(longitud, 0);
        while (true) {
            // Construir el intento actual
            intento.clear();
            for (int i = 0; i < longitud; ++i) {
                intento += alfabeto[indices[i]];
            }
            if (intento == objetivo) {
                auto fin = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duracion = fin - inicio;
                std::cout << "¡Contraseña encontrada!" << std::endl;
                std::cout << "Tiempo transcurrido: " << duracion.count() << " segundos." << std::endl;
                return intento;
            }
            // Incrementar los índices (como un contador en base n)
            int pos = longitud - 1;
            while (pos >= 0) {
                indices[pos]++;
                if (indices[pos] < n) {
                    break;
                } else {
                    indices[pos] = 0;
                    pos--;
                }
            }
            if (pos < 0) break; // Probamos todas las combinaciones de esta longitud
        }
    }
    auto fin = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duracion = fin - inicio;
    std::cout << "No se pudo encontrar la contraseña." << std::endl;
    std::cout << "Tiempo transcurrido: " << duracion.count() << " segundos." << std::endl;
    return "";
}

// Lee la contraseña mostrando asteriscos en vez de los caracteres reales
std::string leerContraseña() {
    std::string pass;
#ifdef _WIN32
    char ch;
    while ((ch = _getch()) != '\r') { // Enter
        if (ch == '\b') { // Backspace
            if (!pass.empty()) {
                pass.pop_back();
                printf("\b \b");
            }
        } else if (std::isprint(static_cast<unsigned char>(ch))) {
            pass += ch;
            printf("*");
        }
    }
#else
    // En otros sistemas, solo usar std::cin (sin ocultar)
    std::cin >> pass;
#endif
    printf("\n");
    return pass;
}

int main() {

    printf("Bienvenido al cracker de contraseñas\n");
    printf("Hecho por: Lucas Moran\n");
    std::string contraseña;
    std::string alfanumericos = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@#$";
    
    // Solicitar la contraseña objetivo
    std::cout << "Ingrese la contraseña objetivo: ";
    contraseña = leerContraseña();

    int opcion;
    std::cout << "\nElige el tipo de ataque:\n";
    std::cout << "1. Fuerza bruta\n";
    std::cout << "2. Ataque por taque por fuerza bruta incremental\n";
   
    std::cout << "Elija una opcion: ";
    std::cin >> opcion;

    switch (opcion) {
        case 1:
            std::cout << "Elegiste fuerza bruta\n";
            {
                std::string encontrada = fuerzaBrutaMultiHilo(contraseña, alfanumericos);
                if (!encontrada.empty()) {
                    std::cout << "La contraseña es: " << encontrada << std::endl;
                }
            }
            break;
        case 2:
            std::cout << "Elegiste ataque por fuerza bruta incremental\n";
            {
                std::string encontrada = fuerzaBrutaIncremental(contraseña, alfanumericos);
                if (encontrada.empty()) {
                    std::cout << "no se pudo encontrar la contraseña" << std::endl; 
                } else {
                    std::cout << "La contraseña es: " << encontrada << std::endl;
                }
            }
            break;
        case 3:
        
        default:
            std::cout << "Opción no válida\n";
            break;
        }
    return 0;
}