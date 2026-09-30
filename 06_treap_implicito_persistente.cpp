// =====================================================================
// 06 - SECUENCIA PERSISTENTE CONFLUENTE (treap implícito + path copying)
// ---------------------------------------------------------------------
// Es un "arreglo/lista" persistente donde la llave es la POSICIÓN
// (implícita, se calcula con tam). Todo se hace con split y merge, y
// ambos copian solo los nodos del camino => O(lg n) esperado.
//
//   insertar(v, pos, x)  borrar(v, pos)  obtener(v, pos)  asignar(v, pos, x)
//   sumaRango(v, l, r)   subsecuencia(v, l, r)
//   concat(v1, v2)  <-- combina 2 versiones cualesquiera => CONFLUENTE
//
// Detalle importante: un treap normal guarda una prioridad aleatoria en
// cada nodo, pero si concatenas una versión consigo misma los nodos se
// repiten y las prioridades dejan de ser independientes. Por eso aquí el
// merge decide la raíz al azar con probabilidad tam(a) / (tam(a)+tam(b))
// (sin prioridades guardadas) -> sigue siendo O(lg n) esperado.
//
// Demo de confluencia: concatenar una versión consigo misma u veces da
// 2^u elementos usando solo O(u lg) nodos nuevos (lo que dice la teoría:
// hasta 2^u "formas" de combinar -> por eso nodos gordos no alcanzan).
// =====================================================================
#include <iostream>
#include <vector>
#include <random>
using namespace std;

typedef long long T;
mt19937 rng(2026);

struct NodoT {
    T val;
    long long tam;
    T suma;            // agregado del subárbol (cámbialo por min/max si lo necesitas)
    NodoT* izq;
    NodoT* der;
};

long long tam(NodoT* n) { return n ? n->tam : 0; }
T suma(NodoT* n) { return n ? n->suma : 0; }

NodoT* actualizar(NodoT* n) {
    n->tam = 1 + tam(n->izq) + tam(n->der);
    n->suma = n->val + suma(n->izq) + suma(n->der);
    return n;
}
NodoT* copiar(NodoT* n) { return new NodoT(*n); }
NodoT* hoja(T x) { return new NodoT{x, 1, x, nullptr, nullptr}; }

// a = primeros k elementos, b = el resto (copiando el camino)
void split(NodoT* n, long long k, NodoT*& a, NodoT*& b) {
    if (!n) { a = b = nullptr; return; }
    NodoT* c = copiar(n);
    if (tam(n->izq) >= k) {
        split(n->izq, k, a, c->izq);
        b = actualizar(c);
    } else {
        split(n->der, k - tam(n->izq) - 1, c->der, b);
        a = actualizar(c);
    }
}

NodoT* merge(NodoT* a, NodoT* b) {
    if (!a) return b;
    if (!b) return a;
    uniform_int_distribution<long long> dist(0, tam(a) + tam(b) - 1);
    if (dist(rng) < tam(a)) {
        NodoT* c = copiar(a);
        c->der = merge(a->der, b);
        return actualizar(c);
    } else {
        NodoT* c = copiar(b);
        c->izq = merge(a, b->izq);
        return actualizar(c);
    }
}

struct SecuenciaPersistente {
    vector<NodoT*> raiz;
    SecuenciaPersistente() { raiz.push_back(nullptr); }     // versión 0 = vacía

    int nueva(NodoT* r) { raiz.push_back(r); return (int)raiz.size() - 1; }

    int desdeVector(const vector<T>& a) {
        NodoT* r = nullptr;
        for (T x : a) r = merge(r, hoja(x));
        return nueva(r);
    }

    // insertar x para que quede en la posición pos (0-indexado)
    int insertar(int v, long long pos, T x) {
        NodoT *a, *b;
        split(raiz[v], pos, a, b);
        return nueva(merge(merge(a, hoja(x)), b));
    }
    int borrar(int v, long long pos) {
        NodoT *a, *b, *m, *c;
        split(raiz[v], pos, a, b);
        split(b, 1, m, c);
        return nueva(merge(a, c));
    }
    int asignar(int v, long long pos, T x) {
        NodoT *a, *b, *m, *c;
        split(raiz[v], pos, a, b);
        split(b, 1, m, c);
        return nueva(merge(merge(a, hoja(x)), c));
    }
    int concat(int v1, int v2) { return nueva(merge(raiz[v1], raiz[v2])); }
    int subsecuencia(int v, long long l, long long r) {      // [l, r]
        NodoT *a, *b, *m, *c;
        split(raiz[v], l, a, b);
        split(b, r - l + 1, m, c);
        return nueva(m);
    }

    // consultas (no crean versión, no copian)
    T obtener(int v, long long pos) {
        NodoT* n = raiz[v];
        while (true) {
            long long ti = tam(n->izq);
            if (pos == ti) return n->val;
            if (pos < ti) n = n->izq;
            else { pos -= ti + 1; n = n->der; }
        }
    }
    T sumaPrefijo(NodoT* n, long long k) {                  // suma de los primeros k
        T s = 0;
        while (n && k > 0) {
            if (tam(n->izq) >= k) n = n->izq;
            else { s += suma(n->izq) + n->val; k -= tam(n->izq) + 1; n = n->der; }
        }
        return s;
    }
    T sumaRango(int v, long long l, long long r) {
        return sumaPrefijo(raiz[v], r + 1) - sumaPrefijo(raiz[v], l);
    }
    long long tamano(int v) { return tam(raiz[v]); }

    void imprimirRec(NodoT* n) {
        if (!n) return;
        imprimirRec(n->izq); cout << n->val << " "; imprimirRec(n->der);
    }
    void imprimir(int v) { cout << "v" << v << ": "; imprimirRec(raiz[v]); cout << "\n"; }
};

int main() {
    SecuenciaPersistente S;
    int v1 = S.desdeVector({1, 2, 3, 4, 5});
    int v2 = S.insertar(v1, 2, 99);      // 1 2 99 3 4 5
    int v3 = S.borrar(v1, 0);            // 2 3 4 5
    int v4 = S.asignar(v2, 4, -7);       // 1 2 99 3 -7 5
    int v5 = S.concat(v3, v4);           // 2 3 4 5 1 2 99 3 -7 5   (confluente)
    int v6 = S.subsecuencia(v5, 3, 6);   // 5 1 2 99

    for (int v : {v1, v2, v3, v4, v5, v6}) S.imprimir(v);
    cout << "suma v5 [2,6] = " << S.sumaRango(v5, 2, 6) << " (4+5+1+2+99=111)\n";

    // --- confluencia: duplicar u veces ---
    int v = S.desdeVector({1, 2, 3});
    for (int u = 0; u < 40; u++) v = S.concat(v, v);
    cout << "tras 40 auto-concats: tam = " << S.tamano(v) << " (= 3 * 2^40)"
         << ", elemento en pos 10^12 = " << S.obtener(v, 1000000000000LL) << "\n";
    return 0;
}
