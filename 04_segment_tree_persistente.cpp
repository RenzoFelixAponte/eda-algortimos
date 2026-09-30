// segment tree persistente (path copying)
// update copia un nodo por nivel -> O(lg n) tiempo y espacio
// tambien sirve como arreglo persistente: get(v,i) = query(v,i,i)
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

typedef long long Valor;
const Valor NEUTRO = 0;                                  // suma -> 0, min -> +INF, max -> -INF
Valor combinar(Valor a, Valor b) { return a + b; }       // cambia aquí la operación

struct NodoST {
    Valor valor;
    NodoST* izq;
    NodoST* der;
};

struct SegTreePersistente {
    int n;
    vector<NodoST*> raiz;

    NodoST* crear(Valor v, NodoST* i, NodoST* d) { return new NodoST{v, i, d}; }

    NodoST* build(const vector<Valor>& a, int l, int r) {
        if (l == r) return crear(a[l], nullptr, nullptr);
        int m = (l + r) / 2;
        NodoST* i = build(a, l, m);
        NodoST* d = build(a, m + 1, r);
        return crear(combinar(i->valor, d->valor), i, d);
    }

    SegTreePersistente(const vector<Valor>& a) {
        n = (int)a.size();
        raiz.push_back(build(a, 0, n - 1));              // versión 0
    }

    // asignar a[pos] = val   (para "sumar" usa: nodo->valor + val en la hoja)
    NodoST* update(NodoST* nodo, int l, int r, int pos, Valor val) {
        if (l == r) return crear(val, nullptr, nullptr);
        int m = (l + r) / 2;
        if (pos <= m) {
            NodoST* i = update(nodo->izq, l, m, pos, val);
            return crear(combinar(i->valor, nodo->der->valor), i, nodo->der);
        } else {
            NodoST* d = update(nodo->der, m + 1, r, pos, val);
            return crear(combinar(nodo->izq->valor, d->valor), nodo->izq, d);
        }
    }

    Valor query(NodoST* nodo, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) return NEUTRO;
        if (ql <= l && r <= qr) return nodo->valor;
        int m = (l + r) / 2;
        return combinar(query(nodo->izq, l, m, ql, qr), query(nodo->der, m + 1, r, ql, qr));
    }

    // ---- interfaz por versiones ----
    int update(int v, int pos, Valor val) {
        raiz.push_back(update(raiz[v], 0, n - 1, pos, val));
        return (int)raiz.size() - 1;
    }
    Valor query(int v, int l, int r) { return query(raiz[v], 0, n - 1, l, r); }
    Valor get(int v, int i) { return query(v, i, i); }

    void imprimir(int v) {
        cout << "v" << v << ": [";
        for (int i = 0; i < n; i++) cout << get(v, i) << (i + 1 < n ? " " : "");
        cout << "]\n";
    }
};

int main() {
    vector<Valor> a = {5, 3, 8, 1, 4, 7};
    SegTreePersistente T(a);

    int v1 = T.update(0, 2, 10);   // a[2] = 10
    int v2 = T.update(v1, 0, 0);   // a[0] = 0
    int v3 = T.update(0, 5, 100);  // rama desde v0 => total

    for (int v : {0, v1, v2, v3}) T.imprimir(v);
    cout << "suma [1,4] en v0=" << T.query(0, 1, 4) << "  v1=" << T.query(v1, 1, 4)
         << "  v2=" << T.query(v2, 1, 4) << "  v3=" << T.query(v3, 1, 4) << "\n";
    cout << "comparte hijo der de la raíz v0 y v1? "
         << (T.raiz[0]->der == T.raiz[v1]->der ? "si" : "no") << "\n";
    return 0;
}
