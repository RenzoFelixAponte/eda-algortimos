// heap binomial (min)
// maximo un arbol de cada grado, como los bits de n
#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>
using namespace std;

long long links = 0;

struct NodoB {
    int key;
    int grado;
    NodoB* padre;
    NodoB* hijo;     // el de mayor grado
    NodoB* hermano;
};

struct HeapBinomial {
    NodoB* cabeza = nullptr;
    int n = 0;

    // y pasa a ser hijo de z
    static void link(NodoB* y, NodoB* z) {
        links++;
        y->padre = z;
        y->hermano = z->hijo;
        z->hijo = y;
        z->grado = z->grado + 1;
    }

    // merge por grado
    static NodoB* mezclarListas(NodoB* a, NodoB* b) {
        NodoB cab;
        cab.key = 0; cab.grado = 0;
        cab.padre = nullptr; cab.hijo = nullptr; cab.hermano = nullptr;
        NodoB* p = &cab;
        while (a != nullptr && b != nullptr) {
            if (a->grado <= b->grado) {
                p->hermano = a;
                a = a->hermano;
            }
            else {
                p->hermano = b;
                b = b->hermano;
            }
            p = p->hermano;
        }
        if (a != nullptr) p->hermano = a;
        else p->hermano = b;
        return cab.hermano;
    }

    static NodoB* unirListas(NodoB* a, NodoB* b) {
        NodoB* h = mezclarListas(a, b);
        if (h == nullptr) return nullptr;
        NodoB* prev = nullptr;
        NodoB* p = h;
        NodoB* q = p->hermano;
        while (q != nullptr) {
            if (p->grado != q->grado || (q->hermano != nullptr && q->hermano->grado == p->grado)) {
                prev = p;
                p = q;
            } else if (p->key <= q->key) {
                p->hermano = q->hermano;
                link(q, p);
            } else {
                if (prev == nullptr) h = q;
                else prev->hermano = q;
                link(p, q);
                p = q;
            }
            q = p->hermano;
        }
        return h;
    }

    void unir(HeapBinomial& otro) {
        cabeza = unirListas(cabeza, otro.cabeza);
        n = n + otro.n;
        otro.cabeza = nullptr;
        otro.n = 0;
    }

    NodoB* insertar(int x) {
        NodoB* p = new NodoB;
        p->key = x; p->grado = 0;
        p->padre = nullptr; p->hijo = nullptr; p->hermano = nullptr;
        cabeza = unirListas(cabeza, p);
        n++;
        return p;
    }

    NodoB* raizMin(NodoB** prevOut) {
        NodoB* mejor = cabeza;
        NodoB* prevMejor = nullptr;
        NodoB* prev = nullptr;
        NodoB* p = cabeza;
        while (p != nullptr) {
            if (p->key < mejor->key) {
                mejor = p;
                prevMejor = prev;
            }
            prev = p;
            p = p->hermano;
        }
        if (prevOut != nullptr) *prevOut = prevMejor;
        return mejor;
    }
    int minimo() { return raizMin(nullptr)->key; }
    bool vacio() { return cabeza == nullptr; }

    int extraerMin() {
        NodoB* prev;
        NodoB* m = raizMin(&prev);
        if (prev == nullptr) cabeza = m->hermano;
        else prev->hermano = m->hermano;
        // invertir hijos
        NodoB* inv = nullptr;
        NodoB* c = m->hijo;
        while (c != nullptr) {
            NodoB* s = c->hermano;
            c->hermano = inv;
            c->padre = nullptr;
            inv = c;
            c = s;
        }
        cabeza = unirListas(cabeza, inv);
        n--;
        int r = m->key;
        delete m;
        return r;
    }

    // ojo: cambia las llaves, no los nodos
    void decreaseKey(NodoB* x, int k) {
        x->key = k;
        NodoB* p = x;
        NodoB* q = p->padre;
        while (q != nullptr && p->key < q->key) {
            int aux = p->key;
            p->key = q->key;
            q->key = aux;
            p = q;
            q = p->padre;
        }
    }
    void eliminar(NodoB* x) {
        decreaseKey(x, INT_MIN);
        extraerMin();
    }

    void imprimirRaices() {
        cout << "raíces (grado:llave): ";
        for (NodoB* p = cabeza; p != nullptr; p = p->hermano)
            cout << "B" << p->grado << ":" << p->key << " ";
        cout << " | n=" << n << "\n";
    }
};

int main() {
    HeapBinomial H;
    int a[7] = {10, 3, 7, 1, 8, 12, 5};
    for (int i = 0; i < 7; i++) H.insertar(a[i]);
    H.imprimirRaices();

    HeapBinomial G;
    G.insertar(6); G.insertar(2); G.insertar(9);
    H.unir(G);
    H.imprimirRaices();

    cout << "extraerMin: ";
    while (!H.vacio()) cout << H.extraerMin() << " ";
    cout << "\n";

    // links / n deberia ser < 1
    HeapBinomial B;
    links = 0;
    int N = 1000000;
    for (int i = 0; i < N; i++) B.insertar(i);
    cout << "links por insert (n=" << N << "): " << (double)links / N << "  -> O(1) amortizado\n";
    return 0;
}
