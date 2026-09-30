// arbol AVL: |h(izq) - h(der)| <= 1 -> altura O(lg n)
// casos LL, LR, RR, RL
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
    return 0;
}
