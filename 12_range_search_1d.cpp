// =====================================================================
// 12 - ORTHOGONAL RANGE SEARCH en 1 DIMENSIÓN  — Semana 4
// ---------------------------------------------------------------------
// Problema: dados n números, ¿cuáles caen en [l, r]?
//   existencia (¿hay alguno?) | conteo (¿cuántos?) | enumeración (¿cuáles?)
//
// (A) Arreglo ordenado + búsqueda binaria (solo sirve en 1D, estático)
//     conteo O(lg n), enumeración O(lg n + k)
//
// (B) BST balanceado (base del Range Tree): buscar el nodo SPLIT (donde
//     los caminos a l y a r se separan). Desde el split:
//       - camino a l: cada vez que voy a la IZQUIERDA, todo el subárbol
//         DERECHO cuelga dentro del rango -> se reporta entero
//       - camino a r: simétrico
//     Hay O(lg n) subárboles "colgados" => enumeración O(lg n + k),
//     conteo O(lg n) si cada nodo guarda tam. Se generaliza a d dimensiones.
// =====================================================================
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

typedef int Llave;

// ---------------- (A) arreglo ordenado ----------------
struct Rango1DArreglo {
    vector<Llave> a;
    Rango1DArreglo(vector<Llave> v) : a(v) { sort(a.begin(), a.end()); }
    int contar(Llave l, Llave r) {
        return (int)(upper_bound(a.begin(), a.end(), r) - lower_bound(a.begin(), a.end(), l));
    }
    bool existe(Llave l, Llave r) { return contar(l, r) > 0; }
    vector<Llave> reportar(Llave l, Llave r) {
        vector<Llave> out;
        for (auto it = lower_bound(a.begin(), a.end(), l); it != a.end() && *it <= r; ++it) out.push_back(*it);
        return out;
    }
};

// ---------------- (B) BST balanceado con split ----------------
struct Nodo {
    Llave llave;
    int tam;
    Nodo* izq;
    Nodo* der;
};
int tam(Nodo* n) { return n ? n->tam : 0; }

struct Rango1DBST {
    Nodo* raiz;
    Nodo* build(const vector<Llave>& a, int l, int r) {       // mediana como raíz
        if (l > r) return nullptr;
        int m = (l + r) / 2;
        Nodo* n = new Nodo{a[m], 0, build(a, l, m - 1), build(a, m + 1, r)};
        n->tam = 1 + tam(n->izq) + tam(n->der);
        return n;
    }
    Rango1DBST(vector<Llave> v) { sort(v.begin(), v.end()); raiz = build(v, 0, (int)v.size() - 1); }

    Nodo* split(Llave l, Llave r) {
        Nodo* v = raiz;
        while (v && (r < v->llave || v->llave < l))
            v = (r < v->llave) ? v->izq : v->der;
        return v;
    }
    void reportarTodo(Nodo* n, vector<Llave>& out) {
        if (!n) return;
        reportarTodo(n->izq, out); out.push_back(n->llave); reportarTodo(n->der, out);
    }

    vector<Llave> reportar(Llave l, Llave r) {
        vector<Llave> out;
        Nodo* s = split(l, r);
        if (!s) return out;
        // camino izquierdo (hacia l)
        vector<Llave> izqPart;
        for (Nodo* v = s->izq; v;) {
            if (l <= v->llave) {                          // v y su subárbol derecho entran
                vector<Llave> tmp; reportarTodo(v->der, tmp);
                tmp.insert(tmp.begin(), v->llave);
                izqPart.insert(izqPart.begin(), tmp.begin(), tmp.end());
                v = v->izq;
            } else v = v->der;
        }
        out = izqPart;
        out.push_back(s->llave);
        // camino derecho (hacia r)
        for (Nodo* v = s->der; v;) {
            if (v->llave <= r) {                          // subárbol izquierdo y v entran
                reportarTodo(v->izq, out);
                out.push_back(v->llave);
                v = v->der;
            } else v = v->izq;
        }
        return out;
    }

    int contar(Llave l, Llave r) {                        // O(lg n) usando tam
        Nodo* s = split(l, r);
        if (!s) return 0;
        int c = 1;
        for (Nodo* v = s->izq; v;) {
            if (l <= v->llave) { c += 1 + tam(v->der); v = v->izq; } else v = v->der;
        }
        for (Nodo* v = s->der; v;) {
            if (v->llave <= r) { c += 1 + tam(v->izq); v = v->der; } else v = v->izq;
        }
        return c;
    }
};

int main() {
    vector<Llave> pts = {2, 6, 7, 23, 42, 52, 68, 71};     // el ejemplo de tu Notion
    Rango1DArreglo A(pts);
    Rango1DBST B(pts);

    cout << "raíz del BST = " << B.raiz->llave << "\n";
    cout << "split de [5, 50] = " << B.split(5, 50)->llave << "\n";
    cout << "[5, 50] -> ";
    for (Llave x : B.reportar(5, 50)) cout << x << " ";
    cout << "| conteo BST=" << B.contar(5, 50) << " arreglo=" << A.contar(5, 50) << "\n";

    // prueba aleatoria
    srand(3);
    vector<Llave> v;
    for (int i = 0; i < 500; i++) v.push_back(rand() % 1000);
    Rango1DArreglo AA(v); Rango1DBST BB(v); bool ok = true;
    for (int q = 0; q < 5000; q++) {
        int l = rand() % 1100 - 50, r = rand() % 1100 - 50; if (l > r) swap(l, r);
        if (AA.reportar(l, r) != BB.reportar(l, r) || AA.contar(l, r) != BB.contar(l, r)) ok = false;
    }
    cout << "prueba aleatoria: " << (ok ? "OK" : "FALLO") << "\n";
    return 0;
}
