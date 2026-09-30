// =====================================================================
// 17 - ÁRBOL AVL (BST balanceado)  — referencia de la Semana 6
// ---------------------------------------------------------------------
// Invariante: |altura(izq) - altura(der)| <= 1 en todo nodo
//   => altura O(lg n) => buscar / insertar / eliminar O(lg n) peor caso.
// Un BST balanceado NO aprende del patrón de accesos: cada Buscar(x)
// cuesta la profundidad de x (comparar con Splay, archivo 18).
//
// Casos de rebalanceo (bal = h(izq) - h(der)):
//   LL: bal>1 y hijo izq con bal>=0  -> rotar der
//   LR: bal>1 y hijo izq con bal<0   -> rotar izq el hijo, luego der
//   RR / RL simétricos
// =====================================================================
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cstdlib>
using namespace std;

typedef int Llave;

struct NodoA {
    Llave llave;
    int altura;
    NodoA* izq;
    NodoA* der;
};

int h(NodoA* n) { return n ? n->altura : 0; }
void actualizar(NodoA* n) { n->altura = 1 + max(h(n->izq), h(n->der)); }
int balance(NodoA* n) { return h(n->izq) - h(n->der); }

NodoA* rotarDer(NodoA* y) {
    NodoA* x = y->izq;
    y->izq = x->der;
    x->der = y;
    actualizar(y); actualizar(x);
    return x;
}
NodoA* rotarIzq(NodoA* x) {
    NodoA* y = x->der;
    x->der = y->izq;
    y->izq = x;
    actualizar(x); actualizar(y);
    return y;
}
NodoA* rebalancear(NodoA* n) {
    actualizar(n);
    int b = balance(n);
    if (b > 1) {
        if (balance(n->izq) < 0) n->izq = rotarIzq(n->izq);   // LR
        return rotarDer(n);                                   // LL
    }
    if (b < -1) {
        if (balance(n->der) > 0) n->der = rotarDer(n->der);   // RL
        return rotarIzq(n);                                   // RR
    }
    return n;
}

NodoA* insertar(NodoA* n, Llave x) {
    if (!n) return new NodoA{x, 1, nullptr, nullptr};
    if (x < n->llave) n->izq = insertar(n->izq, x);
    else if (n->llave < x) n->der = insertar(n->der, x);
    else return n;
    return rebalancear(n);
}

NodoA* quitarMin(NodoA* n, NodoA*& minimo) {
    if (!n->izq) { minimo = n; return n->der; }
    n->izq = quitarMin(n->izq, minimo);
    return rebalancear(n);
}

NodoA* eliminar(NodoA* n, Llave x) {
    if (!n) return nullptr;
    if (x < n->llave) n->izq = eliminar(n->izq, x);
    else if (n->llave < x) n->der = eliminar(n->der, x);
    else {
        NodoA* l = n->izq; NodoA* r = n->der;
        delete n;
        if (!r) return l;
        NodoA* m;
        r = quitarMin(r, m);          // sucesor
        m->izq = l; m->der = r;
        return rebalancear(m);
    }
    return rebalancear(n);
}

bool buscar(NodoA* n, Llave x) {
    while (n) {
        if (x == n->llave) return true;
        n = (x < n->llave) ? n->izq : n->der;
    }
    return false;
}

void inorden(NodoA* n, vector<Llave>& out) {
    if (!n) return;
    inorden(n->izq, out); out.push_back(n->llave); inorden(n->der, out);
}
bool esAVL(NodoA* n) {
    if (!n) return true;
    return abs(balance(n)) <= 1 && n->altura == 1 + max(h(n->izq), h(n->der)) && esAVL(n->izq) && esAVL(n->der);
}

int main() {
    NodoA* r = nullptr;
    for (int i = 1; i <= 1023; i++) r = insertar(r, i);        // entrada ordenada (peor caso de un BST normal)
    cout << "1023 inserts ordenados -> altura " << h(r) << " (un BST normal tendría 1023)\n";

    // prueba aleatoria vs std::set
    srand(4);
    NodoA* t = nullptr; set<Llave> s; bool ok = true;
    for (int i = 0; i < 50000; i++) {
        Llave x = rand() % 2000;
        if (rand() % 3) { t = insertar(t, x); s.insert(x); }
        else { t = eliminar(t, x); s.erase(x); }
        if (i % 1000 == 0) {
            vector<Llave> v; inorden(t, v);
            if (v != vector<Llave>(s.begin(), s.end()) || !esAVL(t)) ok = false;
        }
    }
    cout << "prueba aleatoria: " << (ok ? "OK" : "FALLO") << "  altura final " << h(t) << " con n=" << s.size() << "\n";
    return 0;
}
