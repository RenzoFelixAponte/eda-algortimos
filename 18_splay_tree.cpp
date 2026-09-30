// splay tree: cada acceso sube el nodo a la raiz
// zig (padre es raiz), zig-zig (misma direccion), zig-zag (direcciones opuestas)
// O(lg n) amortizado
#include <iostream>
#include <vector>
#include <set>
#include <cstdlib>
using namespace std;

typedef int Llave;
long long rotaciones = 0;

struct NodoS {
    Llave llave;
    NodoS* izq;
    NodoS* der;
    NodoS* padre;
};

struct SplayTree {
    NodoS* raiz = nullptr;
    int n = 0;

    // rota x con su padre (x sube un nivel)
    void rotar(NodoS* x) {
        rotaciones++;
        NodoS* p = x->padre;
        NodoS* g = p->padre;
        if (p->izq == x) {               // rotación derecha
            p->izq = x->der;
            if (x->der) x->der->padre = p;
            x->der = p;
        } else {                         // rotación izquierda
            p->der = x->izq;
            if (x->izq) x->izq->padre = p;
            x->izq = p;
        }
        p->padre = x;
        x->padre = g;
        if (!g) raiz = x;
        else if (g->izq == p) g->izq = x;
        else g->der = x;
    }

    void splay(NodoS* x) {
        while (x->padre) {
            NodoS* p = x->padre;
            NodoS* g = p->padre;
            if (!g) rotar(x);                                       // Zig
            else if ((g->izq == p) == (p->izq == x)) { rotar(p); rotar(x); }  // Zig-Zig
            else { rotar(x); rotar(x); }                            // Zig-Zag
        }
    }

    // busca x; hace splay del último nodo visitado (aunque no esté x)
    bool buscar(Llave x) {
        NodoS* v = raiz; NodoS* ult = nullptr;
        while (v) {
            ult = v;
            if (x == v->llave) break;
            v = (x < v->llave) ? v->izq : v->der;
        }
        if (ult) splay(ult);
        return v != nullptr;
    }

    void insertar(Llave x) {
        NodoS* v = raiz; NodoS* p = nullptr;
        while (v) {
            p = v;
            if (x == v->llave) { splay(v); return; }
            v = (x < v->llave) ? v->izq : v->der;
        }
        NodoS* nuevo = new NodoS{x, nullptr, nullptr, p};
        if (!p) raiz = nuevo;
        else if (x < p->llave) p->izq = nuevo;
        else p->der = nuevo;
        n++;
        splay(nuevo);
    }

    void eliminar(Llave x) {
        if (!buscar(x)) return;                  // x queda en la raíz
        NodoS* r = raiz;
        NodoS* L = r->izq; NodoS* R = r->der;
        if (L) L->padre = nullptr;
        if (R) R->padre = nullptr;
        delete r; n--;
        if (!L) { raiz = R; return; }
        // el máximo de L sube a la raíz de L (no tendrá hijo derecho) y se cuelga R
        raiz = L;
        NodoS* m = L;
        while (m->der) m = m->der;
        splay(m);
        m->der = R;
        if (R) R->padre = m;
    }

    int profundidad(Llave x) {
        int d = 0;
        for (NodoS* v = raiz; v; v = (x < v->llave) ? v->izq : v->der, d++)
            if (v->llave == x) return d;
        return -1;
    }
    void inorden(NodoS* v, vector<Llave>& out) {
        if (!v) return;
        inorden(v->izq, out); out.push_back(v->llave); inorden(v->der, out);
    }
};

int main() {
    SplayTree T;
    for (int x : {10, 20, 30, 40, 50}) T.insertar(x);   // insertar en orden deja una "línea"
    cout << "raíz tras insertar 10..50 = " << T.raiz->llave << ", profundidad de 10 = " << T.profundidad(10) << "\n";
    T.buscar(10);
    cout << "tras buscar(10): raíz = " << T.raiz->llave << ", profundidad de 50 = " << T.profundidad(50) << "\n";
    return 0;
}
