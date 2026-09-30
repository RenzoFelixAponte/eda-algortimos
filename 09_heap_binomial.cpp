// =====================================================================
// 09 - MONTÍCULO BINOMIAL (min)
// ---------------------------------------------------------------------
// Colección de árboles binomiales B_k (B_k tiene 2^k nodos, raíz de
// grado k) con A LO MUCHO UNO de cada grado -> como la representación
// binaria de n. Lista de raíces ordenada por grado.
//
//   link(y, z)    : y (llave mayor) pasa a ser hijo de z.  B_k + B_k = B_{k+1}
//   unir(H1, H2)  : merge de listas por grado + "suma binaria con acarreo"  O(lg n)
//   insertar      : unir con un B_0.  O(lg n) peor caso, O(1) AMORTIZADO
//                   (Ejercicio 6: es como incrementar un contador binario;
//                    potencial Φ = # árboles.)
//   extraerMin    : quitar la raíz mínima, invertir su lista de hijos, unir.  O(lg n)
//   decreaseKey   : subir intercambiando llaves con el padre.  O(lg n)
//   eliminar      : decreaseKey a -INF + extraerMin.  O(lg n)
// =====================================================================
#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>
using namespace std;

typedef int Llave;
long long links = 0;

struct NodoB {
    Llave llave;
    int grado;
    NodoB* padre;
    NodoB* hijo;       // hijo de MAYOR grado
    NodoB* hermano;    // siguiente en la lista (raíces o hermanos)
};

struct HeapBinomial {
    NodoB* cabeza = nullptr;   // lista de raíces, grados crecientes
    int n = 0;

    static void link(NodoB* y, NodoB* z) {   // y hijo de z (y->llave >= z->llave)
        links++;
        y->padre = z;
        y->hermano = z->hijo;
        z->hijo = y;
        z->grado++;
    }

    // mezcla dos listas de raíces por grado (como merge de mergesort)
    static NodoB* mezclarListas(NodoB* a, NodoB* b) {
        NodoB cab{0, 0, nullptr, nullptr, nullptr};
        NodoB* t = &cab;
        while (a && b) {
            if (a->grado <= b->grado) { t->hermano = a; a = a->hermano; }
            else                      { t->hermano = b; b = b->hermano; }
            t = t->hermano;
        }
        t->hermano = a ? a : b;
        return cab.hermano;
    }

    static NodoB* unirListas(NodoB* a, NodoB* b) {
        NodoB* h = mezclarListas(a, b);
        if (!h) return nullptr;
        NodoB* prev = nullptr;
        NodoB* x = h;
        NodoB* sig = x->hermano;
        while (sig) {
            if (x->grado != sig->grado ||
                (sig->hermano && sig->hermano->grado == x->grado)) {
                prev = x; x = sig;                      // avanzar
            } else if (x->llave <= sig->llave) {
                x->hermano = sig->hermano;              // sig bajo x
                link(sig, x);
            } else {
                if (!prev) h = sig; else prev->hermano = sig;
                link(x, sig);                           // x bajo sig
                x = sig;
            }
            sig = x->hermano;
        }
        return h;
    }

    void unir(HeapBinomial& otro) {       // destruye "otro"
        cabeza = unirListas(cabeza, otro.cabeza);
        n += otro.n;
        otro.cabeza = nullptr; otro.n = 0;
    }

    NodoB* insertar(Llave x) {
        NodoB* nodo = new NodoB{x, 0, nullptr, nullptr, nullptr};
        cabeza = unirListas(cabeza, nodo);
        n++;
        return nodo;
    }

    NodoB* raizMin(NodoB** prevOut) {
        NodoB *mejor = cabeza, *prevMejor = nullptr, *prev = nullptr;
        for (NodoB* x = cabeza; x; prev = x, x = x->hermano)
            if (x->llave < mejor->llave) { mejor = x; prevMejor = prev; }
        if (prevOut) *prevOut = prevMejor;
        return mejor;
    }
    Llave minimo() { return raizMin(nullptr)->llave; }
    bool vacio() { return cabeza == nullptr; }

    Llave extraerMin() {
        NodoB* prev;
        NodoB* m = raizMin(&prev);
        if (!prev) cabeza = m->hermano; else prev->hermano = m->hermano;
        // hijos de m están en grado decreciente -> invertir
        NodoB* inv = nullptr;
        for (NodoB* c = m->hijo; c;) {
            NodoB* s = c->hermano;
            c->hermano = inv; c->padre = nullptr;
            inv = c; c = s;
        }
        cabeza = unirListas(cabeza, inv);
        n--;
        Llave r = m->llave;
        delete m;
        return r;
    }

    // OJO: intercambia LLAVES, así que el puntero "x" puede quedar con otra llave.
    // Si necesitas handles estables, guarda un campo "dato" y muévelo junto a la llave.
    void decreaseKey(NodoB* x, Llave k) {
        x->llave = k;
        NodoB* y = x;
        NodoB* z = y->padre;
        while (z && y->llave < z->llave) {
            swap(y->llave, z->llave);
            y = z; z = y->padre;
        }
    }
    void eliminar(NodoB* x) { decreaseKey(x, INT_MIN); extraerMin(); }

    void imprimirRaices() {
        cout << "raíces (grado:llave): ";
        for (NodoB* x = cabeza; x; x = x->hermano) cout << "B" << x->grado << ":" << x->llave << " ";
        cout << " | n=" << n << "\n";
    }
};

int main() {
    HeapBinomial H;
    for (int x : {10, 3, 7, 1, 8, 12, 5}) H.insertar(x);   // n = 7 = 111b -> B0, B1, B2
    H.imprimirRaices();

    HeapBinomial G;
    for (int x : {6, 2, 9}) G.insertar(x);
    H.unir(G);                                              // n = 10 = 1010b -> B1, B3
    H.imprimirRaices();

    cout << "extraerMin: ";
    while (!H.vacio()) cout << H.extraerMin() << " ";
    cout << "\n";

    // Insert O(1) amortizado: links totales / n inserciones < 1
    HeapBinomial B;
    links = 0;
    int N = 1000000;
    for (int i = 0; i < N; i++) B.insertar(i);
    cout << "links por insert (n=" << N << "): " << (double)links / N << "  -> O(1) amortizado\n";
    return 0;
}
