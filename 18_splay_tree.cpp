// splay tree
// zig, zig-zig, zig-zag
#include <iostream>
#include <vector>
#include <set>
#include <cstdlib>
using namespace std;

long long rotaciones = 0;

struct NodoS {
    int key;
    NodoS* left;
    NodoS* right;
    NodoS* padre;
};

struct SplayTree {
    NodoS* raiz = nullptr;
    int n = 0;

    // sube x un nivel
    void rotar(NodoS* x) {
        rotaciones++;
        NodoS* p = x->padre;
        NodoS* g = p->padre;
        if (p->left == x) {
            p->left = x->right;
            if (x->right != nullptr) x->right->padre = p;
            x->right = p;
        }
        else {
            p->right = x->left;
            if (x->left != nullptr) x->left->padre = p;
            x->left = p;
        }
        p->padre = x;
        x->padre = g;
        if (g == nullptr) raiz = x;
        else if (g->left == p) g->left = x;
        else g->right = x;
    }

    void splay(NodoS* x) {
        while (x->padre != nullptr) {
            NodoS* p = x->padre;
            NodoS* g = p->padre;
            if (g == nullptr) {
                rotar(x);  // zig
            }
            else {
                bool n1 = (g->left == p);
                bool n2 = (p->left == x);
                if (n1 == n2) {
                    // zig zig
                    rotar(p);
                    rotar(x);
                } else {
                    rotar(x);
                    rotar(x);
                }
            }
        }
    }

    bool buscar(int x) {
        NodoS* p = raiz;
        NodoS* ult = nullptr;
        while (p != nullptr) {
            ult = p;
            if (x == p->key) break;
            if (x < p->key) p = p->left;
            else p = p->right;
        }
        if (ult != nullptr) splay(ult);
        return p != nullptr;
    }

    void insertar(int x) {
        NodoS* p = raiz;
        NodoS* q = nullptr;
        while (p != nullptr) {
            q = p;
            if (x == p->key) {
                splay(p);
                return;
            }
            if (x < p->key) p = p->left;
            else p = p->right;
        }
        NodoS* aux = new NodoS;
        aux->key = x;
        aux->left = nullptr;
        aux->right = nullptr;
        aux->padre = q;
        if (q == nullptr) raiz = aux;
        else if (x < q->key) q->left = aux;
        else q->right = aux;
        n++;
        splay(aux);
    }

    void eliminar(int x) {
        if (!buscar(x)) return;  // x queda en la raiz
        NodoS* r = raiz;
        NodoS* L = r->left;
        NodoS* R = r->right;
        if (L != nullptr) L->padre = nullptr;
        if (R != nullptr) R->padre = nullptr;
        delete r;
        n--;
        if (L == nullptr) {
            raiz = R;
            return;
        }
        raiz = L;
        NodoS* m = L;
        while (m->right != nullptr) m = m->right;
        splay(m);
        m->right = R;
        if (R) R->padre = m;
    }

    int profundidad(int x) {
        int d = 0;
        NodoS* p = raiz;
        while (p != nullptr) {
            if (p->key == x) return d;
            if (x < p->key) p = p->left;
            else p = p->right;
            d++;
        }
        return -1;
    }
    void inorden(NodoS* p, vector<int>& res) {
        if (p == nullptr) return;
        inorden(p->left, res);
        res.push_back(p->key);
        inorden(p->right, res);
    }
};

int main() {
    SplayTree T;
    int a[5] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) T.insertar(a[i]);
    cout << "raíz tras insertar 10..50 = " << T.raiz->key << ", profundidad de 10 = " << T.profundidad(10) << "\n";
    T.buscar(10);
    cout << "tras buscar(10): raíz = " << T.raiz->key << ", profundidad de 50 = " << T.profundidad(50) << "\n";
    return 0;
}
