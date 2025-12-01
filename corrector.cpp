#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
using namespace std;

#define TAMTOKEN 50

// Alfabeto a utilizar para las operaciones
const char alfabeto[] = "abcdefghijklmnñopqrstuvwxyzáéíóú";
const int TAM_ALFABETO = 32;

// ==================== FUNCIONES AUXILIARES ====================

// Convierte una cadena a minúsculas
void aMinusculas(char* cadena) {
    for (int i = 0; cadena[i] != '\0'; i++) {
        cadena[i] = tolower(cadena[i]);
    }
}

// Verifica si un carácter es un delimitador
bool esDelimitador(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || 
            c == ',' || c == '.' || c == ';' || c == '(' || c == ')');
}

// Elimina signos de puntuación al final de una palabra
void eliminarPuntuacion(char* palabra) {
    int len = strlen(palabra);
    while (len > 0 && (palabra[len-1] == ',' || palabra[len-1] == '.' || 
           palabra[len-1] == ';' || palabra[len-1] == '(' || palabra[len-1] == ')')) {
        palabra[len-1] = '\0';
        len--;
    }
}

// Búsqueda binaria en el diccionario
int busquedaBinaria(char szPalabras[][TAMTOKEN], int numElementos, const char* palabra) {
    int izq = 0;
    int der = numElementos - 1;
    
    while (izq <= der) {
        int medio = (izq + der) / 2;
        int cmp = strcmp(szPalabras[medio], palabra);
        
        if (cmp == 0) {
            return medio; // Encontrada
        } else if (cmp < 0) {
            izq = medio + 1;
        } else {
            der = medio - 1;
        }
    }
    return -1; // No encontrada
}

// Inserta una palabra en orden alfabético
void insertarOrdenado(char szPalabras[][TAMTOKEN], int iEstadisticas[], 
                      int& numElementos, const char* palabra) {
    // Buscar posición donde insertar
    int pos = 0;
    while (pos < numElementos && strcmp(szPalabras[pos], palabra) < 0) {
        pos++;
    }
    
    // Mover elementos hacia adelante
    for (int i = numElementos; i > pos; i--) {
        strcpy(szPalabras[i], szPalabras[i-1]);
        iEstadisticas[i] = iEstadisticas[i-1];
    }
    
    // Insertar nueva palabra
    strcpy(szPalabras[pos], palabra);
    iEstadisticas[pos] = 1;
    numElementos++;
}

// ==================== FUNCIÓN 1: DICCIONARIO ====================

void Diccionario(
    char* szNombre,
    char szPalabras[][TAMTOKEN],
    int iEstadisticas[],
    int& iNumElementos)
{
    iNumElementos = 0;
    
    // Abrir archivo
    ifstream archivo(szNombre);
    if (!archivo.is_open()) {
        return;
    }
    
    char palabra[TAMTOKEN];
    char c;
    int idx = 0;
    
    // Leer archivo carácter por carácter
    while (archivo.get(c)) {
        if (esDelimitador(c)) {
            if (idx > 0) {
                palabra[idx] = '\0';
                eliminarPuntuacion(palabra);
                aMinusculas(palabra);
                
                if (strlen(palabra) > 0) {
                    // Buscar si ya existe
                    int pos = busquedaBinaria(szPalabras, iNumElementos, palabra);
                    
                    if (pos != -1) {
                        // Ya existe, incrementar frecuencia
                        iEstadisticas[pos]++;
                    } else {
                        // No existe, insertar ordenado
                        insertarOrdenado(szPalabras, iEstadisticas, iNumElementos, palabra);
                    }
                }
                idx = 0;
            }
        } else {
            if (idx < TAMTOKEN - 1) {
                palabra[idx++] = c;
            }
        }
    }
    
    // Procesar última palabra si existe
    if (idx > 0) {
        palabra[idx] = '\0';
        eliminarPuntuacion(palabra);
        aMinusculas(palabra);
        
        if (strlen(palabra) > 0) {
            int pos = busquedaBinaria(szPalabras, iNumElementos, palabra);
            
            if (pos != -1) {
                iEstadisticas[pos]++;
            } else {
                insertarOrdenado(szPalabras, iEstadisticas, iNumElementos, palabra);
            }
        }
    }
    
    archivo.close();
}

// ==================== FUNCIÓN 2: CLONAR PALABRAS ====================

void ClonaPalabras(
    char* szPalabraLeida,
    char szPalabrasSugeridas[][TAMTOKEN],
    int& iNumSugeridas)
{
    iNumSugeridas = 0;
    int len = strlen(szPalabraLeida);
    
    // Añadir la palabra original
    strcpy(szPalabrasSugeridas[iNumSugeridas++], szPalabraLeida);
    
    // ===== 1. ELIMINACIÓN =====
    // Eliminar cada carácter de la palabra
    for (int i = 0; i < len; i++) {
        char temp[TAMTOKEN];
        int idx = 0;
        
        for (int j = 0; j < len; j++) {
            if (j != i) {
                temp[idx++] = szPalabraLeida[j];
            }
        }
        temp[idx] = '\0';
        
        if (strlen(temp) > 0) {
            strcpy(szPalabrasSugeridas[iNumSugeridas++], temp);
        }
    }
    
    // ===== 2. TRANSPOSICIÓN =====
    // Intercambiar caracteres adyacentes
    for (int i = 0; i < len - 1; i++) {
        char temp[TAMTOKEN];
        strcpy(temp, szPalabraLeida);
        
        // Intercambiar temp[i] con temp[i+1]
        char aux = temp[i];
        temp[i] = temp[i+1];
        temp[i+1] = aux;
        
        strcpy(szPalabrasSugeridas[iNumSugeridas++], temp);
    }
    
    // ===== 3. SUSTITUCIÓN =====
    // Sustituir cada carácter por cada letra del alfabeto
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < TAM_ALFABETO; j++) {
            char temp[TAMTOKEN];
            strcpy(temp, szPalabraLeida);
            temp[i] = alfabeto[j];
            strcpy(szPalabrasSugeridas[iNumSugeridas++], temp);
        }
    }
    
    // ===== 4. INSERCIÓN =====
    // Insertar cada letra del alfabeto en cada posición
    // Antes del primer carácter
    for (int j = 0; j < TAM_ALFABETO; j++) {
        char temp[TAMTOKEN];
        temp[0] = alfabeto[j];
        strcpy(temp + 1, szPalabraLeida);
        strcpy(szPalabrasSugeridas[iNumSugeridas++], temp);
    }
    
    // Entre caracteres (esto también cubre después del último carácter cuando i = len-1)
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < TAM_ALFABETO; j++) {
            char temp[TAMTOKEN];
            int idx = 0;
            
            // Copiar hasta la posición i (inclusive)
            for (int k = 0; k <= i; k++) {
                temp[idx++] = szPalabraLeida[k];
            }
            
            // Insertar letra del alfabeto
            temp[idx++] = alfabeto[j];
            
            // Copiar el resto
            for (int k = i + 1; k < len; k++) {
                temp[idx++] = szPalabraLeida[k];
            }
            temp[idx] = '\0';
            
            strcpy(szPalabrasSugeridas[iNumSugeridas++], temp);
        }
    }
    
    // Ordenar las palabras sugeridas alfabéticamente
    // Ordenamiento burbuja simple
    for (int i = 0; i < iNumSugeridas - 1; i++) {
        for (int j = 0; j < iNumSugeridas - i - 1; j++) {
            if (strcmp(szPalabrasSugeridas[j], szPalabrasSugeridas[j+1]) > 0) {
                char temp[TAMTOKEN];
                strcpy(temp, szPalabrasSugeridas[j]);
                strcpy(szPalabrasSugeridas[j], szPalabrasSugeridas[j+1]);
                strcpy(szPalabrasSugeridas[j+1], temp);
            }
        }
    }
}

// ==================== FUNCIÓN 3: LISTA CANDIDATAS ====================

void ListaCandidatas(
    char szPalabrasSugeridas[][TAMTOKEN],
    int iNumSugeridas,
    char szPalabras[][TAMTOKEN],
    int iEstadisticas[],
    int iNumElementos,
    char szListaFinal[][TAMTOKEN],
    int iPeso[],
    int& iNumLista)
{
    iNumLista = 0;
    
    // Buscar cada palabra sugerida en el diccionario
    for (int i = 0; i < iNumSugeridas; i++) {
        int pos = busquedaBinaria(szPalabras, iNumElementos, szPalabrasSugeridas[i]);
        
        if (pos != -1) {
            // La palabra existe en el diccionario
            // Verificar si ya está en la lista final (evitar duplicados)
            bool existe = false;
            for (int j = 0; j < iNumLista; j++) {
                if (strcmp(szListaFinal[j], szPalabras[pos]) == 0) {
                    existe = true;
                    break;
                }
            }
            
            if (!existe) {
                strcpy(szListaFinal[iNumLista], szPalabras[pos]);
                iPeso[iNumLista] = iEstadisticas[pos];
                iNumLista++;
            }
        }
    }
    
    // Ordenar por peso (frecuencia) de mayor a menor
    for (int i = 0; i < iNumLista - 1; i++) {
        for (int j = 0; j < iNumLista - i - 1; j++) {
            if (iPeso[j] < iPeso[j+1]) {
                // Intercambiar pesos
                int tempPeso = iPeso[j];
                iPeso[j] = iPeso[j+1];
                iPeso[j+1] = tempPeso;
                
                // Intercambiar palabras
                char tempPalabra[TAMTOKEN];
                strcpy(tempPalabra, szListaFinal[j]);
                strcpy(szListaFinal[j], szListaFinal[j+1]);
                strcpy(szListaFinal[j+1], tempPalabra);
            }
        }
    }
}git commit -m "segundo commit"
