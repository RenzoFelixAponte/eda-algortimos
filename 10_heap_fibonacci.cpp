// fibonacci heap (min)
// insert, union y decrease key O(1) amortizado, extract min O(lg n)
#include <iostream>
#include <vector>
#include <climits>
#include <cmath>
using namespace std;

struct Nodo {
    int key;
    Nodo* padre;
    Nodo* hijo;
    Nodo* left;
    Nodo* right;
    int grado;
    bool marca;
};

struct FibHeap {
    Nodo* min = nullptr;
    int n = 0;

    // mete x a la izquierda de lista
    static void meterEnLista(Nodo* lista, Nodo* x) {
        x->right = lista;
        x->left = lista->left;
        lista->left->right = x;
        lista->left = x;
    }
    static void sacarDeLista(Nodo* x) {
        x->left->right = x->right;
        x->right->left = x->left;
        x->left = x;
        x->right = x;
    }

    void insertarEnRaices(Nodo* x) {
        x->padre = nullptr;
        if (min == nullptr) {
            x->left = x;
            x->right = x;
            min = x;
        }
        else {
            meterEnLista(min, x);
            if (x->key < min->key) min = x;
        }
    }

    Nodo* insertar(int k) {
        Nodo* p = new Nodo;
        p->key = k;
        p->padre = nullptr; p->hijo = nullptr;
        p->grado = 0; p->marca = false;
        p->left = p;
        p->right = p;
        insertarEnRaices(p);
        n++;
        return p;
    }

    int minimo() { return min->key; }
    bool vacio() { return min == nullptr; }

    void unir(FibHeap& H2) {
        if (H2.min == nullptr) return;
        if (min == nullptr) {
            min = H2.min;
            n = H2.n;
        } else {
            Nodo* a = min->right;
            Nodo* b = H2.min->left;
            min->right = H2.min;
            H2.min->left = min;
            a->left = b;
            b->right = a;
            if (H2.min->key < min->key) min = H2.min;
            n += H2.n;
        }
        H2.min = nullptr;
        H2.n = 0;
    }

    // y se vuelve hijo de x
    void binomialLink(Nodo* y, Nodo* x) {
        sacarDeLista(y);
        if (x->hijo == nullptr) {
            x->hijo = y;
            y->left = y;
            y->right = y;
        }
        else meterEnLista(x->hijo, y);
        y->padre = x;
        y->marca = false;
        x->grado++;
    }

    void consolidate() {
        int maxGrado = (int)(log2((double)n) * 1.45) + 2;
        vector<Nodo*> A(maxGrado + 1, nullptr);

        // guardo las raices primero porque la lista cambia
        vector<Nodo*> raices;
        Nodo* p = min;
        do {
            raices.push_back(p);
            p = p->right;
        } while (p != min);

        for (int i = 0; i < (int)raices.size(); i++) {
            p = raices[i];
            int d = p->grado;
            while (A[d] != nullptr) {
                Nodo* q = A[d];
                if (q->key < p->key) {
                    Nodo* aux = p;
                    p = q;
                    q = aux;
                }
                binomialLink(q, p);
                A[d] = nullptr;
                d++;
            }
            A[d] = p;
        }
        min = nullptr;
        for (int i = 0; i < (int)A.size(); i++) {
            if (A[i] != nullptr) {
                A[i]->left = A[i];
                A[i]->right = A[i];
                insertarEnRaices(A[i]);
            }
        }
    }

    int extraerMin() {
        Nodo* z = min;
        if (z->hijo != nullptr) {
            vector<Nodo*> hijos;
            Nodo* c = z->hijo;
            do {
                hijos.push_back(c);
                c = c->right;
            } while (c != z->hijo);
            for (int i = 0; i < (int)hijos.size(); i++) {
                sacarDeLista(hijos[i]);
                hijos[i]->padre = nullptr;
                meterEnLista(min, hijos[i]);
            }
        }
        if (z->right == z) {
            min = nullptr;
        } else {
            min = z->right;
            sacarDeLista(z);
            consolidate();
        }
        n--;
        int r = z->key;
        delete z;
        return r;
    }

    void cut(Nodo* x, Nodo* y) {
        if (x->right == x) y->hijo = nullptr;
        else {
            if (y->hijo == x) y->hijo = x->right;
            sacarDeLista(x);
        }
        y->grado--;
        x->marca = false;
        x->left = x;
        x->right = x;
        insertarEnRaices(x);
    }
    void cascadingCut(Nodo* y) {
        Nodo* z = y->padre;
        if (z != nullptr) {
            if (y->marca == false) {
                y->marca = true;
            }
            else {
                cut(y, z);
                cascadingCut(z);
            }
        }
    }
    void decreaseKey(Nodo* x, int k) {
        x->key = k;
        Nodo* y = x->padre;
        if (y != nullptr && x->key < y->key) {
            cut(x, y);
            cascadingCut(y);
        }
        if (x->key < min->key) min = x;
    }

    void eliminar(Nodo* x) {
        decreaseKey(x, INT_MIN);
        extraerMin();
    }

    int numArboles() {
        if (min == nullptr) return 0;
        int t = 0;
        Nodo* p = min;
        do {
            t++;
            p = p->right;
        } while (p != min);
        return t;
    }
};

int main() {
    FibHeap H;
    vector<Nodo*> h;
    int a[15] = {23, 7, 21, 3, 18, 52, 38, 39, 41, 17, 30, 24, 26, 46, 35};
    for (int i = 0; i < 15; i++) h.push_back(H.insertar(a[i]));
    cout << "tras 15 inserts: árboles en la raíz = " << H.numArboles() << " (no consolida)\n";
    cout << "extraerMin = " << H.extraerMin() << ", árboles tras consolidar = " << H.numArboles() << "\n";

    H.decreaseKey(h[13], 15);
    H.decreaseKey(h[14], 5);
    H.eliminar(h[4]);  // el 18

    FibHeap G;
    G.insertar(4);
    G.insertar(100);
    H.unir(G);

    cout << "extraerMin: ";
    while (!H.vacio()) cout << H.extraerMin() << " ";
    cout << "\n";
    return 0;
}
