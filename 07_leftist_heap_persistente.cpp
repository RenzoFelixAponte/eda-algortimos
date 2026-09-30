// =====================================================================
// 07 - MONTÍCULO PERSISTENTE (Leftist Heap funcional)
// ---------------------------------------------------------------------
// Cola de prioridad persistente CONFLUENTE: se pueden unir 2 versiones.
// Todo se reduce a merge(a, b), que baja SOLO por la espina derecha
// (longitud O(lg n) por la propiedad leftist) copiando esos nodos.
//
// Propiedad leftist: rango(izq) >= rango(der), donde rango(x) = largo
// del camino más a la derecha hasta null. => rango(raíz) <= lg(n+1).
//
//   insertar(v, x)  = merge(v, {x})          O(lg n)
//   extraerMin(v)   = merge(raiz->izq, der)   O(lg n)  (no copia la raíz)
//   union(v1, v2)   = merge(v1, v2)           O(lg n)  <-- confluente
//   min(v)                                    O(1)
//
// (Comparar con Fibonacci Heap de la semana 2: Fibonacci modifica
//  punteros en su lugar -> NO es persistente tal cual.)
// =====================================================================
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

typedef int Llave;

struct NodoH {
    Llave llave;
    int rango;
    NodoH* izq;
    NodoH* der;
};

int rango(NodoH* n) { return n ? n->rango : 0; }

NodoH* mergeH(NodoH* a, NodoH* b) {
    if (!a) return b;
    if (!b) return a;
    if (b->llave < a->llave) swap(a, b);                  // a tiene el mínimo (min-heap)
    NodoH* c = new NodoH(*a);                             // copia del nodo del camino
    c->der = mergeH(a->der, b);
    if (rango(c->izq) < rango(c->der)) swap(c->izq, c->der);
    c->rango = rango(c->der) + 1;
    return c;
}

struct HeapPersistente {
    vector<NodoH*> raiz;
    HeapPersistente() { raiz.push_back(nullptr); }

    int nueva(NodoH* r) { raiz.push_back(r); return (int)raiz.size() - 1; }

    int insertar(int v, Llave x) { return nueva(mergeH(raiz[v], new NodoH{x, 1, nullptr, nullptr})); }
    int extraerMin(int v) {
        if (!raiz[v]) return nueva(nullptr);
        return nueva(mergeH(raiz[v]->izq, raiz[v]->der));
    }
    int unir(int v1, int v2) { return nueva(mergeH(raiz[v1], raiz[v2])); }
    bool vacio(int v) { return raiz[v] == nullptr; }
    Llave minimo(int v) { return raiz[v]->llave; }

    // lista ordenada de la versión (no la modifica: usa versiones temporales)
    vector<Llave> ordenado(int v) {
        vector<Llave> out;
        NodoH* r = raiz[v];
        while (r) { out.push_back(r->llave); r = mergeH(r->izq, r->der); }
        return out;
    }
    void imprimir(int v) {
        cout << "v" << v << ": ";
        for (Llave x : ordenado(v)) cout << x << " ";
        cout << "\n";
    }
};

int main() {
    HeapPersistente H;
    int a = 0;
    for (int x : {7, 3, 9, 1}) a = H.insertar(a, x);     // a = {1,3,7,9}
    int b = 0;
    for (int x : {8, 2, 6}) b = H.insertar(b, x);        // b = {2,6,8}
    int c = H.extraerMin(a);                             // {3,7,9}
    int d = H.unir(a, b);                                // {1,2,3,6,7,8,9}  confluente
    int e = H.unir(d, d);                                // unión consigo misma

    for (int v : {a, b, c, d, e}) H.imprimir(v);
    cout << "min(a)=" << H.minimo(a) << " sigue intacto tras extraerMin\n";
    return 0;
}
