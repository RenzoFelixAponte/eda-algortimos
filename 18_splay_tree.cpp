// =====================================================================
// 18 - SPLAY TREE  — Semana 6 (Dynamic Optimality I)
// ---------------------------------------------------------------------
// Cada vez que accedo a x lo SUBO A LA RAÍZ con rotaciones (splay):
//   Zig     : p es la raíz                       -> rotar x una vez
//   Zig-Zig : x y p en la MISMA dirección (línea)-> rotar p con g, luego x con p
//   Zig-Zag : direcciones OPUESTAS (codo)        -> rotar x dos veces
//
// O(lg n) amortizado por operación. Cumple: acceso secuencial,
// dynamic finger, working set y cota de entropía (optimalidad estática).
// ¿Propiedad unificada? no se sabe. ¿O(1)-competitivo? conjetura abierta.
//
// Con padre explícito (modelo BST de la clase: padre, izq, der).
// =====================================================================
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

    // acceso secuencial: O(1) amortizado por acceso
    int N = 100000;
    SplayTree S;
    for (int i = 0; i < N; i++) S.insertar(rand());
    vector<Llave> llaves; S.inorden(S.raiz, llaves);
    rotaciones = 0;
    for (int rep = 0; rep < 3; rep++) for (Llave x : llaves) S.buscar(x);
    cout << "acceso secuencial: rotaciones por búsqueda = " << (double)rotaciones / (3.0 * llaves.size()) << " (O(1))\n";

    // working set: repetir pocas llaves es barato
    rotaciones = 0;
    for (int i = 0; i < 300000; i++) S.buscar(llaves[i % 8 * 1000]);
    cout << "working set de 8 llaves: rotaciones por búsqueda = " << rotaciones / 300000.0 << " (O(lg 8))\n";

    // prueba aleatoria vs std::set
    srand(8);
    SplayTree R; set<Llave> ref; bool ok = true;
    for (int i = 0; i < 50000; i++) {
        Llave x = rand() % 3000; int op = rand() % 3;
        if (op == 0) { R.insertar(x); ref.insert(x); }
        else if (op == 1) { R.eliminar(x); ref.erase(x); }
        else if (R.buscar(x) != (ref.count(x) > 0)) ok = false;
    }
    vector<Llave> v; R.inorden(R.raiz, v);
    if (v != vector<Llave>(ref.begin(), ref.end())) ok = false;
    cout << "prueba aleatoria: " << (ok ? "OK" : "FALLO") << "\n";
    return 0;
}
