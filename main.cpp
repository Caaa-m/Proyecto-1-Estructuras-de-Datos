/*
 * ============================================================
 * ST0245 - Estructuras de Datos y Algoritmos
 * Práctica I: Ordenación de Dataset Grande (C++)
 * ============================================================
 * Este programa ordena 100,000 palabras usando tres estrategias:
 *   1. QuickSort   -> sobre std::vector<string>
 *   2. HeapSort    -> usando un Binary Max-Heap manual
 *   3. AVL Tree    -> árbol balanceado, recorrido inorder
 *
 * Para cada estrategia se mide:
 *   - Tiempo de ejecución (std::chrono)
 *   - Estimación de memoria usada
 * ============================================================
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>    // para medir tiempos
#include <iomanip>   // para formatear la salida (setw, fixed, etc.)
#include <cmath>     // para log2
#include <algorithm> // para min con initializer_list

using namespace std;
using namespace std::chrono;

// ============================================================
//  SECCIÓN 1: QUICKSORT
//  Implementación del algoritmo QuickSort sobre std::vector<string>
// ============================================================

/*
 * partition: función auxiliar de QuickSort.
 *
 * Toma el elemento en la posición 'high' como PIVOTE.
 * Recorre el subarreglo [low..high-1] y mueve hacia la izquierda
 * todos los elementos que sean MENORES O IGUALES al pivote.
 * Al final coloca el pivote en su posición correcta y retorna ese índice.
 *
 * Complejidad: O(n) — recorre el subarreglo una sola vez.
 *
 * Parámetros:
 *   arr  -> vector de strings que se está ordenando
 *   low  -> índice izquierdo del subarreglo actual
 *   high -> índice derecho del subarreglo actual (= pivote)
 *
 * Retorna: índice final del pivote después de la partición
 */
int partition(vector<string>& arr, int low, int high) {
    string pivot = arr[high];  // elegimos el último elemento como pivote
    int i = low - 1;           // i apunta a la última posición de la "zona menor"

    for (int j = low; j < high; j++) {
        // Si el elemento actual es menor o igual al pivote,
        // lo movemos a la zona izquierda (zona de elementos <= pivote)
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);  // intercambio: arr[i] <-> arr[j]
        }
    }
    // Colocamos el pivote en su posición definitiva
    swap(arr[i + 1], arr[high]);
    return i + 1;  // índice donde quedó el pivote
}

/*
 * quickSort: implementación recursiva del algoritmo QuickSort.
 *
 * Divide el arreglo en dos mitades usando 'partition',
 * luego ordena recursivamente cada mitad.
 *
 * Complejidad:
 *   - Mejor caso:  O(n log n) — pivote divide el arreglo en mitades iguales
 *   - Caso promedio: O(n log n)
 *   - Peor caso:   O(n²)     — cuando el arreglo ya está ordenado
 *                              y siempre elegimos el último como pivote
 *
 * Parámetros:
 *   arr  -> vector de strings
 *   low  -> índice izquierdo del subarreglo a ordenar
 *   high -> índice derecho del subarreglo a ordenar
 */
void quickSort(vector<string>& arr, int low, int high) {
    if (low < high) {
        // pi = índice del pivote después de la partición
        int pi = partition(arr, low, high);

        // Ordenar recursivamente la mitad izquierda (elementos < pivote)
        quickSort(arr, low, pi - 1);

        // Ordenar recursivamente la mitad derecha (elementos > pivote)
        quickSort(arr, pi + 1, high);
    }
}


// ============================================================
//  SECCIÓN 2: HEAPSORT
//  Implementación de HeapSort usando un Binary Max-Heap manual
// ============================================================

/*
 * heapify: función clave para mantener la propiedad del Max-Heap.
 *
 * Un Max-Heap es un árbol binario completo donde cada nodo padre
 * es MAYOR O IGUAL que sus hijos.
 * En representación de arreglo:
 *   - hijo izquierdo de nodo i  -> 2*i + 1
 *   - hijo derecho de nodo i    -> 2*i + 2
 *   - padre de nodo i           -> (i-1) / 2
 *
 * Esta función asume que los subárboles izquierdo y derecho de 'i'
 * ya son Max-Heaps. Si arr[i] no es el mayor, lo "hunde" hacia abajo
 * hasta restaurar la propiedad del heap.
 *
 * Complejidad: O(log n) — en el peor caso baja hasta la hoja.
 *
 * Parámetros:
 *   arr  -> vector que representa el heap
 *   n    -> tamaño del heap (no siempre es arr.size())
 *   i    -> nodo raíz del subárbol a "heapificar"
 */
void heapify(vector<string>& arr, int n, int i) {
    int largest = i;        // asumimos que la raíz es el mayor
    int left    = 2 * i + 1;  // índice hijo izquierdo
    int right   = 2 * i + 2;  // índice hijo derecho

    // Verificar si el hijo izquierdo existe Y es mayor que la raíz actual
    if (left < n && arr[left] > arr[largest]) {
        largest = left;
    }

    // Verificar si el hijo derecho existe Y es mayor que el mayor actual
    if (right < n && arr[right] > arr[largest]) {
        largest = right;
    }

    // Si la raíz NO es el mayor, intercambiar y seguir "hundiendo"
    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);  // llamada recursiva en el subárbol afectado
    }
}

/*
 * heapSort: implementación del algoritmo HeapSort.
 *
 * Funciona en dos fases:
 *   FASE 1 - BUILD HEAP: convierte el arreglo en un Max-Heap.
 *     Partimos desde el último nodo interno (n/2 - 1) hacia la raíz.
 *     Complejidad: O(n)
 *
 *   FASE 2 - EXTRACT MAX: extrae el máximo (raíz) uno a uno.
 *     Mueve la raíz (máximo) al final del arreglo y reduce el tamaño del heap.
 *     Repara el heap con heapify en la nueva raíz.
 *     Complejidad: O(n log n)
 *
 * Complejidad total: O(n log n) en todos los casos (mejor, promedio, peor).
 * Ventaja: no usa memoria adicional (in-place).
 *
 * Parámetros:
 *   arr -> vector de strings a ordenar
 */
void heapSort(vector<string>& arr) {
    int n = arr.size();

    // --- FASE 1: Construir el Max-Heap ---
    // El último nodo interno está en (n/2 - 1).
    // Los nodos desde n/2 hasta n-1 son hojas (ya son heaps triviales).
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }

    // --- FASE 2: Extraer elementos del heap uno a uno ---
    // En cada iteración, el máximo (arr[0]) se lleva al final del arreglo.
    // Luego se restaura el heap sobre los n-1 elementos restantes.
    for (int i = n - 1; i > 0; i--) {
        // Mover el máximo actual (raíz) al final
        swap(arr[0], arr[i]);

        // Restaurar la propiedad del heap sobre el heap reducido (tamaño i)
        heapify(arr, i, 0);
    }
    // Al terminar, el arreglo está ordenado ascendentemente.
}


// ============================================================
//  SECCIÓN 3: AVL TREE
//  Árbol binario de búsqueda auto-balanceado (Adelson-Velsky & Landis)
// ============================================================

/*
 * AVLNode: nodo del árbol AVL.
 *
 * Cada nodo almacena:
 *   - key:    la palabra (string)
 *   - left:   puntero al hijo izquierdo
 *   - right:  puntero al hijo derecho
 *   - height: altura del nodo (para calcular el factor de balance)
 *
 * Factor de balance (BF) = altura(izquierdo) - altura(derecho)
 * Un árbol AVL mantiene: -1 <= BF <= 1 para TODOS sus nodos.
 * Si BF sale de ese rango, se aplican rotaciones para re-balancear.
 */
struct AVLNode {
    string   key;
    AVLNode* left;
    AVLNode* right;
    int      height;

    // Constructor
    AVLNode(const string& k)
        : key(k), left(nullptr), right(nullptr), height(1) {}
};

/*
 * avlHeight: retorna la altura de un nodo.
 * Si el nodo es nulo, retorna 0 (convención para hojas nulas).
 */
int avlHeight(AVLNode* node) {
    return (node == nullptr) ? 0 : node->height;
}

/*
 * avlGetBalance: calcula el factor de balance de un nodo.
 * BF > 1  => subárbol izquierdo muy alto => necesita rotación derecha
 * BF < -1 => subárbol derecho muy alto   => necesita rotación izquierda
 */
int avlGetBalance(AVLNode* node) {
    return (node == nullptr) ? 0 : avlHeight(node->left) - avlHeight(node->right);
}

/*
 * avlUpdateHeight: recalcula la altura de un nodo basándose en sus hijos.
 * altura(nodo) = 1 + max(altura(izq), altura(der))
 */
void avlUpdateHeight(AVLNode* node) {
    if (node != nullptr) {
        node->height = 1 + max(avlHeight(node->left), avlHeight(node->right));
    }
}

/*
 * rotateRight: rotación simple a la derecha.
 *
 * Se aplica cuando el subárbol IZQUIERDO es muy alto (BF > 1).
 *
 * Antes:        Después:
 *     y              x
 *    / \            / \
 *   x   T3         T1   y
 *  / \                 / \
 * T1  T2              T2  T3
 *
 * Parámetros:
 *   y -> nodo desbalanceado (raíz del subárbol a rotar)
 * Retorna: nueva raíz del subárbol (x)
 */
AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x  = y->left;    // x sube a ser la nueva raíz
    AVLNode* T2 = x->right;   // subárbol intermedio (T2)

    // Realizar la rotación
    x->right = y;   // y pasa a ser hijo derecho de x
    y->left  = T2;  // T2 pasa a ser hijo izquierdo de y

    // Actualizar alturas (primero y, luego x, porque y ahora es hijo de x)
    avlUpdateHeight(y);
    avlUpdateHeight(x);

    return x;  // nueva raíz del subárbol
}

/*
 * rotateLeft: rotación simple a la izquierda.
 *
 * Se aplica cuando el subárbol DERECHO es muy alto (BF < -1).
 *
 * Antes:        Después:
 *   x                y
 *  / \              / \
 * T1   y           x   T3
 *     / \         / \
 *    T2  T3      T1  T2
 */
AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y  = x->right;
    AVLNode* T2 = y->left;

    // Realizar la rotación
    y->left  = x;
    x->right = T2;

    // Actualizar alturas
    avlUpdateHeight(x);
    avlUpdateHeight(y);

    return y;  // nueva raíz del subárbol
}

/*
 * avlInsert: inserta una palabra en el AVL Tree y re-balancea si es necesario.
 *
 * Proceso:
 *   1. Inserción normal de BST (como árbol binario de búsqueda estándar).
 *   2. Actualizar altura del nodo actual.
 *   3. Calcular factor de balance.
 *   4. Si está desbalanceado, aplicar la rotación correcta.
 *
 * Hay 4 casos de desbalanceo:
 *   - Left-Left (LL):   BF > 1 y key < nodo->left->key  => rotateRight
 *   - Right-Right (RR): BF < -1 y key > nodo->right->key => rotateLeft
 *   - Left-Right (LR):  BF > 1 y key > nodo->left->key  => rotateLeft(left) + rotateRight
 *   - Right-Left (RL):  BF < -1 y key < nodo->right->key => rotateRight(right) + rotateLeft
 *
 * Complejidad: O(log n) — el árbol siempre está balanceado, altura = O(log n).
 *
 * Parámetros:
 *   node -> raíz del subárbol donde insertar
 *   key  -> palabra a insertar
 * Retorna: nueva raíz del subárbol (puede cambiar por rotaciones)
 */
AVLNode* avlInsert(AVLNode* node, const string& key) {
    // --- Paso 1: Inserción estándar BST ---
    if (node == nullptr) {
        return new AVLNode(key);  // caso base: crear nuevo nodo
    }

    if (key < node->key) {
        node->left = avlInsert(node->left, key);   // insertar en subárbol izquierdo
    } else if (key > node->key) {
        node->right = avlInsert(node->right, key); // insertar en subárbol derecho
    } else {
        return node;  // duplicados no se insertan (no requerido en el enunciado)
    }

    // --- Paso 2: Actualizar altura del nodo actual ---
    avlUpdateHeight(node);

    // --- Paso 3: Calcular factor de balance ---
    int balance = avlGetBalance(node);

    // --- Paso 4: Re-balancear si es necesario ---

    // Caso Left-Left (LL): inserción fue en el subárbol izquierdo del hijo izquierdo
    if (balance > 1 && key < node->left->key) {
        return rotateRight(node);
    }

    // Caso Right-Right (RR): inserción fue en el subárbol derecho del hijo derecho
    if (balance < -1 && key > node->right->key) {
        return rotateLeft(node);
    }

    // Caso Left-Right (LR): inserción fue en el subárbol derecho del hijo izquierdo
    // Primero rotar el hijo izquierdo a la izquierda, luego rotar la raíz a la derecha
    if (balance > 1 && key > node->left->key) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // Caso Right-Left (RL): inserción fue en el subárbol izquierdo del hijo derecho
    // Primero rotar el hijo derecho a la derecha, luego rotar la raíz a la izquierda
    if (balance < -1 && key < node->right->key) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;  // sin desbalanceo, retornar nodo sin cambios
}

/*
 * avlInorder: recorrido inorder del AVL Tree.
 *
 * El recorrido INORDER visita los nodos en orden: izquierdo -> raíz -> derecho.
 * En un BST (y por ende en un AVL), esto produce los elementos en orden ASCENDENTE.
 *
 * Complejidad: O(n) — visita cada nodo exactamente una vez.
 *
 * Parámetros:
 *   node   -> nodo actual (comienza desde la raíz)
 *   result -> vector donde se acumulan las palabras ordenadas
 */
void avlInorder(AVLNode* node, vector<string>& result) {
    if (node == nullptr) return;

    avlInorder(node->left, result);    // 1. Visitar subárbol izquierdo
    result.push_back(node->key);       // 2. Registrar el valor del nodo actual
    avlInorder(node->right, result);   // 3. Visitar subárbol derecho
}

/*
 * avlDeleteTree: libera toda la memoria del árbol (post-order).
 *
 * Post-order: primero los hijos, luego la raíz.
 * Esto evita perder referencias antes de liberar memoria.
 */
void avlDeleteTree(AVLNode* node) {
    if (node == nullptr) return;
    avlDeleteTree(node->left);
    avlDeleteTree(node->right);
    delete node;
}


// ============================================================
//  SECCIÓN 4: UTILIDADES - Lectura de archivo y análisis
// ============================================================

/*
 * loadDataset: lee el archivo dataset.txt y retorna un vector con las palabras.
 *
 * El archivo puede estar en UTF-16 LE (como lo generó el profesor) o en UTF-8.
 * Esta función intenta leer línea por línea y elimina espacios/saltos de línea.
 *
 * Parámetros:
 *   filename -> ruta del archivo de texto
 * Retorna: vector<string> con las palabras leídas
 */
vector<string> loadDataset(const string& filename) {
    vector<string> words;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "ERROR: No se pudo abrir el archivo '" << filename << "'\n";
        cerr << "Asegurate de que 'dataset.txt' este en la misma carpeta que el ejecutable.\n";
        return words;
    }

    string line;
    while (getline(file, line)) {
        // Eliminar caracteres de retorno de carro (\r) si el archivo es Windows
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        // Saltar líneas vacías
        if (!line.empty()) {
            words.push_back(line);
        }
    }

    file.close();
    return words;
}

/*
 * verifyOrder: verifica que el vector esté ordenado correctamente.
 *
 * Recorre el arreglo comparando cada elemento con el siguiente.
 * Si encuentra un par desordenado, retorna false.
 *
 * Parámetros:
 *   arr -> vector a verificar
 * Retorna: true si está ordenado, false si no
 */
bool verifyOrder(const vector<string>& arr) {
    for (size_t i = 1; i < arr.size(); i++) {
        if (arr[i] < arr[i - 1]) {
            return false;  // encontramos un par fuera de orden
        }
    }
    return true;
}

/*
 * printSample: imprime las primeras y últimas 'n' palabras de un vector.
 * Útil para verificar visualmente que el ordenamiento es correcto.
 */
void printSample(const vector<string>& arr, int n = 5) {
    cout << "  Primeras " << n << " palabras: ";
    for (int i = 0; i < n && i < (int)arr.size(); i++) {
        cout << arr[i] << " ";
    }
    cout << "\n  Ultimas  " << n << " palabras: ";
    for (int i = max(0, (int)arr.size() - n); i < (int)arr.size(); i++) {
        cout << arr[i] << " ";
    }
    cout << "\n";
}

/*
 * estimateMemoryQuickSort: estima la memoria usada por QuickSort.
 *
 * QuickSort usa:
 *   - El vector principal: N * sizeof(string) bytes
 *     (en la práctica cada string tiene overhead del objeto + la cadena en heap)
 *   - Stack de recursión: profundidad promedio O(log N), peor caso O(N)
 *     Cada frame de recursión guarda: low, high, pi (~3 ints = 12 bytes)
 *
 * Nota: sizeof(string) en la mayoría de implementaciones es 32 bytes (objeto).
 * El contenido real de la cadena puede estar en el heap (si len > SSO_limit).
 * SSO (Small String Optimization) guarda strings cortos (~15 chars) inline.
 */
void estimateMemoryQuickSort(size_t n) {
    size_t vectorData    = n * sizeof(string);     // tamaño del objeto string * N
    size_t avgStackDepth = (size_t)(log2(n) * 3 * sizeof(int)); // frames en recursión
    size_t totalEstimate = vectorData + avgStackDepth;

    cout << "  Memoria estimada (QuickSort):\n";
    cout << "    - Vector datos (N * sizeof(string) = N * " << sizeof(string) << " bytes): "
         << vectorData / 1024 << " KB\n";
    cout << "    - Stack recursion (O(log N) frames):          "
         << avgStackDepth << " bytes\n";
    cout << "    - TOTAL aprox:                                "
         << totalEstimate / 1024 << " KB\n";
}

/*
 * estimateMemoryHeapSort: estima la memoria usada por HeapSort.
 *
 * HeapSort es IN-PLACE: ordena directamente sobre el vector existente.
 * No requiere estructuras auxiliares.
 *   - El vector: N * sizeof(string) bytes
 *   - heapify() es recursivo pero con profundidad O(log N)
 */
void estimateMemoryHeapSort(size_t n) {
    size_t vectorData    = n * sizeof(string);
    size_t heapifyStack  = (size_t)(log2(n) * 3 * sizeof(int));
    size_t totalEstimate = vectorData + heapifyStack;

    cout << "  Memoria estimada (HeapSort):\n";
    cout << "    - Vector datos (in-place, mismo vector):      "
         << vectorData / 1024 << " KB\n";
    cout << "    - Stack heapify (O(log N) frames):            "
         << heapifyStack << " bytes\n";
    cout << "    - TOTAL aprox:                                "
         << totalEstimate / 1024 << " KB\n";
}

/*
 * estimateMemoryAVL: estima la memoria usada por el AVL Tree.
 *
 * Cada nodo AVL almacena:
 *   - string key:     sizeof(string) bytes (objeto) + contenido de la cadena
 *   - AVLNode* left:  sizeof(pointer) = 8 bytes
 *   - AVLNode* right: sizeof(pointer) = 8 bytes
 *   - int height:     4 bytes
 *   - padding:        ~4 bytes (alineación de memoria)
 * Total por nodo: aprox sizeof(AVLNode) bytes
 *
 * Además, el vector de resultado (inorder) también ocupa N * sizeof(string).
 */
void estimateMemoryAVL(size_t n) {
    size_t nodeSize      = sizeof(AVLNode);  // tamaño real de un nodo
    size_t treeTotal     = n * nodeSize;
    size_t resultVector  = n * sizeof(string);
    size_t totalEstimate = treeTotal + resultVector;

    cout << "  Memoria estimada (AVL Tree):\n";
    cout << "    - sizeof(AVLNode) = " << nodeSize << " bytes/nodo\n";
    cout << "    - " << n << " nodos en el arbol:                     "
         << treeTotal / 1024 << " KB\n";
    cout << "    - Vector resultado inorder:                   "
         << resultVector / 1024 << " KB\n";
    cout << "    - TOTAL aprox:                                "
         << totalEstimate / 1024 << " KB\n";
}


// ============================================================
//  SECCIÓN 5: FUNCIÓN MAIN
//  Orquesta la carga del dataset, ejecución de los 3 algoritmos
//  y la comparación final.
// ============================================================

int main() {
    cout << "=====================================================\n";
    cout << "  ST0245 - Practica I: Ordenacion de Dataset Grande\n";
    cout << "  QuickSort | HeapSort | AVL Tree\n";
    cout << "=====================================================\n\n";

    // --- Cargar el dataset desde archivo ---
    cout << "Cargando dataset desde 'dataset.txt'...\n";
    vector<string> original = loadDataset("dataset.txt");

    if (original.empty()) {
        cerr << "ERROR: El dataset esta vacio o no se pudo leer.\n";
        return 1;
    }

    cout << "Dataset cargado: " << original.size() << " palabras.\n\n";

    // Guardamos una copia "original" para restaurar antes de cada algoritmo,
    // porque cada uno recibe los datos SIN ordenar.
    vector<string> arr;

    // =========================================================
    //  ALGORITMO 1: QUICKSORT
    // =========================================================
    cout << "-----------------------------------------------------\n";
    cout << "  [1] QUICKSORT sobre std::vector<string>\n";
    cout << "-----------------------------------------------------\n";

    arr = original;  // copia fresca del dataset sin ordenar

    // Medir tiempo de inicio
    auto startQS = high_resolution_clock::now();

    // Llamar a quickSort con índices 0 y n-1
    quickSort(arr, 0, (int)arr.size() - 1);

    // Medir tiempo de fin
    auto endQS = high_resolution_clock::now();

    // Calcular duración en milisegundos
    double timeQS = duration_cast<microseconds>(endQS - startQS).count() / 1000.0;

    cout << "  Tiempo de ejecucion: " << fixed << setprecision(3) << timeQS << " ms\n";
    cout << "  Ordenamiento correcto: " << (verifyOrder(arr) ? "SI ✓" : "NO ✗") << "\n";
    printSample(arr);
    estimateMemoryQuickSort(arr.size());
    cout << "\n";


    // =========================================================
    //  ALGORITMO 2: HEAPSORT
    // =========================================================
    cout << "-----------------------------------------------------\n";
    cout << "  [2] HEAPSORT usando Binary Heap\n";
    cout << "-----------------------------------------------------\n";

    arr = original;  // restaurar copia sin ordenar

    auto startHS = high_resolution_clock::now();

    heapSort(arr);

    auto endHS = high_resolution_clock::now();
    double timeHS = duration_cast<microseconds>(endHS - startHS).count() / 1000.0;

    cout << "  Tiempo de ejecucion: " << fixed << setprecision(3) << timeHS << " ms\n";
    cout << "  Ordenamiento correcto: " << (verifyOrder(arr) ? "SI ✓" : "NO ✗") << "\n";
    printSample(arr);
    estimateMemoryHeapSort(arr.size());
    cout << "\n";


    // =========================================================
    //  ALGORITMO 3: AVL TREE (insertar + inorder traversal)
    // =========================================================
    cout << "-----------------------------------------------------\n";
    cout << "  [3] AVL TREE (insercion + recorrido inorder)\n";
    cout << "-----------------------------------------------------\n";

    AVLNode* avlRoot = nullptr;  // raíz del árbol, inicialmente vacío

    auto startAVL = high_resolution_clock::now();

    // Insertar las 100,000 palabras en el AVL Tree
    // Cada inserción: O(log n) => total: O(n log n)
    for (const string& word : original) {
        avlRoot = avlInsert(avlRoot, word);
    }

    // Recuperar el resultado ordenado mediante recorrido inorder
    // El inorder en un BST/AVL siempre produce orden ascendente
    vector<string> avlResult;
    avlResult.reserve(original.size());  // reservar espacio para evitar reallocations
    avlInorder(avlRoot, avlResult);

    auto endAVL = high_resolution_clock::now();
    double timeAVL = duration_cast<microseconds>(endAVL - startAVL).count() / 1000.0;

    cout << "  Tiempo de ejecucion: " << fixed << setprecision(3) << timeAVL << " ms\n";
    cout << "  Palabras en resultado: " << avlResult.size()
         << " (menos si habia duplicados en el dataset)\n";
    cout << "  Ordenamiento correcto: " << (verifyOrder(avlResult) ? "SI ✓" : "NO ✗") << "\n";
    printSample(avlResult);
    estimateMemoryAVL(avlResult.size());

    // Liberar memoria del árbol AVL (importante para evitar memory leaks)
    avlDeleteTree(avlRoot);
    avlRoot = nullptr;
    cout << "\n";


    // =========================================================
    //  ANÁLISIS COMPARATIVO FINAL
    // =========================================================
    cout << "=====================================================\n";
    cout << "  ANALISIS COMPARATIVO\n";
    cout << "=====================================================\n\n";

    // Tabla de tiempos
    cout << "  Tiempos de ejecucion:\n";
    cout << "  +-----------------+--------------+\n";
    cout << "  | Algoritmo       | Tiempo (ms)  |\n";
    cout << "  +-----------------+--------------+\n";
    cout << "  | QuickSort       | " << setw(12) << fixed << setprecision(3) << timeQS  << " |\n";
    cout << "  | HeapSort        | " << setw(12) << fixed << setprecision(3) << timeHS  << " |\n";
    cout << "  | AVL Tree        | " << setw(12) << fixed << setprecision(3) << timeAVL << " |\n";
    cout << "  +-----------------+--------------+\n\n";

    // Determinar el ganador
    double minTime = min(timeQS, min(timeHS, timeAVL));
    string winner;
    if (minTime == timeQS)       winner = "QuickSort";
    else if (minTime == timeHS)  winner = "HeapSort";
    else                          winner = "AVL Tree";
    cout << "  => Algoritmo mas rapido: " << winner << " (" << minTime << " ms)\n\n";

    // Tabla de complejidades
    cout << "  Complejidades algoritmicas (Big O):\n";
    cout << "  +-----------------+-------------+-------------+-------------+-----------+\n";
    cout << "  | Algoritmo       | Mejor caso  | Caso promedio | Peor caso  | Memoria   |\n";
    cout << "  +-----------------+-------------+-------------+-------------+-----------+\n";
    cout << "  | QuickSort       | O(n log n)  | O(n log n)  | O(n^2)      | O(log n)  |\n";
    cout << "  | HeapSort        | O(n log n)  | O(n log n)  | O(n log n)  | O(1)      |\n";
    cout << "  | AVL Tree        | O(n log n)  | O(n log n)  | O(n log n)  | O(n)      |\n";
    cout << "  +-----------------+-------------+-------------+-------------+-----------+\n\n";

    // Conclusiones
    cout << "  CONCLUSIONES:\n\n";
    cout << "  1. QUICKSORT:\n";
    cout << "     - En la practica suele ser el mas rapido debido a su excelente\n";
    cout << "       localidad de cache (accede a memoria de forma contigua).\n";
    cout << "     - Su peor caso O(n^2) ocurre con datos ya ordenados + pivote ultimo.\n";
    cout << "     - Para este dataset aleatorio, su caso promedio O(n log n) se cumple.\n\n";

    cout << "  2. HEAPSORT:\n";
    cout << "     - Garantiza O(n log n) en todos los casos, sin peor caso cuadratico.\n";
    cout << "     - Es in-place (O(1) memoria adicional), ventaja importante.\n";
    cout << "     - En la practica es mas lento que QuickSort por peor uso de cache:\n";
    cout << "       los accesos al heap saltan por el arreglo de forma no contigua.\n\n";

    cout << "  3. AVL TREE:\n";
    cout << "     - Mantiene el arbol siempre balanceado con rotaciones.\n";
    cout << "     - Cada insercion es O(log n) garantizado (altura = O(log n)).\n";
    cout << "     - Usa mas memoria: O(n) nodos con punteros e informacion de altura.\n";
    cout << "     - El overhead de punteros y llamadas recursivas lo hace mas lento\n";
    cout << "       que los otros dos en la practica, aunque la complejidad es igual.\n";
    cout << "     - Ventaja: util cuando se necesita insertar/buscar/eliminar dinamicamente.\n\n";

    cout << "  Por que la teoria difiere de la practica?\n";
    cout << "     - La complejidad Big O ignora constantes multiplicativas.\n";
    cout << "     - La localidad de memoria (cache hits/misses) tiene gran impacto real.\n";
    cout << "     - El overhead de punteros en AVL vs acceso a arreglo contiguo es\n";
    cout << "       significativo con 100,000 elementos.\n";

    cout << "\n=====================================================\n";
    cout << "  Fin del programa.\n";
    cout << "=====================================================\n";

    return 0;
}