// =====================================================================
// 10 - FIBONACCI HEAP (min)  — Semana 2 de tu Notion
// ---------------------------------------------------------------------
// Colección de árboles min-heap; raíces en lista circular doble.
// Invariantes:
//   1) llave(x) >= llave(padre(x))
//   2) un nodo no raíz pierde a lo mucho 1 hijo desde que fue enlazado
//      (marca); si pierde el 2º se corta también (cascading cut)
//   3) => grado máximo D(n) = O(lg n)  (S(x) >= F(k+2))
//
//   Operación      Peor caso   Amortizado      Φ(H) = t(H) + 2 m(H)
//   Find-Min       O(1)        O(1)
//   Insert         O(1)        O(1)
//   Union          O(1)        O(1)
//   Decrease-Key   O(n)        O(1)
//   Extract-Min    O(n)        O(lg n)
//   Delete         O(n)        O(lg n)
// =====================================================================
#include <iostream>
#include <vector>
#include <climits>
#include <cmath>
using namespace std;

typedef int Llave;

struct Nodo {
    Llave llave;
    Nodo* padre;
    Nodo* hijo;
    Nodo* izq;
    Nodo* der;
    int grado;
    bool marca;
};

struct FibHeap {
    Nodo* min = nullptr;
    int n = 0;

    // ---- utilidades de lista circular ----
    static void meterEnLista(Nodo* lista, Nodo* x) {     // x a la izquierda de "lista"
        x->der = lista;
        x->izq = lista->izq;
        lista->izq->der = x;
        lista->izq = x;
    }
    static void sacarDeLista(Nodo* x) {
        x->izq->der = x->der;
        x->der->izq = x->izq;
        x->izq = x->der = x;
    }

    void insertarEnRaices(Nodo* x) {
        x->padre = nullptr;
        if (min == nullptr) { x->izq = x->der = x; min = x; }
        else {
            meterEnLista(min, x);
            if (x->llave < min->llave) min = x;
        }
    }

    // ---- Insert O(1) ----
    Nodo* insertar(Llave k) {
        Nodo* x = new Nodo{k, nullptr, nullptr, nullptr, nullptr, 0, false};
        x->izq = x->der = x;
        insertarEnRaices(x);
        n++;
        return x;
    }

    Llave minimo() { return min->llave; }
    bool vacio() { return min == nullptr; }

    // ---- Union O(1): concatenar listas circulares ----
    void unir(FibHeap& H2) {
        if (H2.min == nullptr) return;
        if (min == nullptr) { min = H2.min; n = H2.n; }
        else {
            Nodo* a = min->der;
            Nodo* b = H2.min->izq;
            min->der = H2.min;  H2.min->izq = min;
            a->izq = b;         b->der = a;
            if (H2.min->llave < min->llave) min = H2.min;
            n += H2.n;
        }
        H2.min = nullptr; H2.n = 0;
    }

    // ---- Link: y pasa a ser hijo de x ----
    void binomialLink(Nodo* y, Nodo* x) {
        sacarDeLista(y);
        if (x->hijo == nullptr) { x->hijo = y; y->izq = y->der = y; }
        else meterEnLista(x->hijo, y);
        y->padre = x;
        y->marca = false;
        x->grado++;
    }

    void consolidate() {
        int maxGrado = (int)(log2((double)n) * 1.45) + 2;   // D(n) <= log_φ n ≈ 1.44 lg n
        vector<Nodo*> A(maxGrado + 1, nullptr);

        // copiar raíces a un arreglo porque la lista cambia mientras recorremos
        vector<Nodo*> raices;
        Nodo* x = min;
        do { raices.push_back(x); x = x->der; } while (x != min);

        for (Nodo* w : raices) {
            x = w;
            int d = x->grado;
            while (A[d] != nullptr) {
                Nodo* y = A[d];
                if (y->llave < x->llave) swap(x, y);
                binomialLink(y, x);
                A[d] = nullptr;
                d++;
            }
            A[d] = x;
        }
        min = nullptr;
        for (Nodo* a : A) if (a) {
            a->izq = a->der = a;
            insertarEnRaices(a);
        }
    }

    // ---- Extract-Min O(lg n) amortizado ----
    Llave extraerMin() {
        Nodo* z = min;
        // hijos de z -> lista de raíces
        if (z->hijo) {
            vector<Nodo*> hijos;
            Nodo* c = z->hijo;
            do { hijos.push_back(c); c = c->der; } while (c != z->hijo);
            for (Nodo* h : hijos) { sacarDeLista(h); h->padre = nullptr; meterEnLista(min, h); }
        }
        if (z->der == z) min = nullptr;          // z era la única raíz
        else {
            min = z->der;
            sacarDeLista(z);
            consolidate();
        }
        n--;
        Llave r = z->llave;
        delete z;
        return r;
    }

    // ---- Decrease-Key O(1) amortizado ----
    void cut(Nodo* x, Nodo* y) {
        if (x->der == x) y->hijo = nullptr;
        else {
            if (y->hijo == x) y->hijo = x->der;
            sacarDeLista(x);
        }
        y->grado--;
        x->marca = false;
        x->izq = x->der = x;
        insertarEnRaices(x);
    }
    void cascadingCut(Nodo* y) {
        Nodo* z = y->padre;
        if (z != nullptr) {
            if (!y->marca) y->marca = true;      // primer hijo perdido: solo marcar
            else { cut(y, z); cascadingCut(z); } // segundo: cortar y propagar
        }
    }
    void decreaseKey(Nodo* x, Llave k) {
        x->llave = k;
        Nodo* y = x->padre;
        if (y != nullptr && x->llave < y->llave) {
            cut(x, y);
            cascadingCut(y);
        }
        if (x->llave < min->llave) min = x;
    }

    void eliminar(Nodo* x) { decreaseKey(x, INT_MIN); extraerMin(); }

    int numArboles() {
        if (!min) return 0;
        int t = 0; Nodo* x = min;
        do { t++; x = x->der; } while (x != min);
        return t;
    }
};

int main() {
    FibHeap H;
    vector<Nodo*> h;
    for (int x : {23, 7, 21, 3, 18, 52, 38, 39, 41, 17, 30, 24, 26, 46, 35}) h.push_back(H.insertar(x));
    cout << "tras 15 inserts: árboles en la raíz = " << H.numArboles() << " (no consolida)\n";
    cout << "extraerMin = " << H.extraerMin() << ", árboles tras consolidar = " << H.numArboles() << "\n";

    H.decreaseKey(h[13], 15);   // 46 -> 15
    H.decreaseKey(h[14], 5);    // 35 -> 5
    H.eliminar(h[4]);           // borra 18

    FibHeap G;
    G.insertar(4); G.insertar(100);
    H.unir(G);

    cout << "extraerMin: ";
    while (!H.vacio()) cout << H.extraerMin() << " ";
    cout << "\n";

    // prueba aleatoria vs ordenar
    FibHeap R; vector<Nodo*> nodos; vector<int> vivos;
    srand(1);
    for (int i = 0; i < 20000; i++) { int k = rand() % 1000000; nodos.push_back(R.insertar(k)); }
    for (int i = 0; i < 20000; i += 3) R.decreaseKey(nodos[i], nodos[i]->llave - rand() % 1000);
    vector<int> sal;
    while (!R.vacio()) sal.push_back(R.extraerMin());
    bool ok = sal.size() == 20000;
    for (size_t i = 1; i < sal.size(); i++) if (sal[i] < sal[i - 1]) ok = false;
    cout << "prueba aleatoria: " << (ok ? "OK" : "FALLO") << "\n";
    return 0;
}
