// leftist heap persistente (min heap)
// todo se hace con merge, copia la espina derecha
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct NodoH {
    int key;
    int rango;
    NodoH* left;
    NodoH* right;
};

int rango(NodoH* p) {
    if (p == nullptr) return 0;
    return p->rango;
}

NodoH* mergeH(NodoH* a, NodoH* b) {
    if (a == nullptr) return b;
    if (b == nullptr) return a;
    if (b->key < a->key) {
        NodoH* aux = a;
        a = b;
        b = aux;
    }
    NodoH* p = new NodoH(*a);  // copia
    p->right = mergeH(a->right, b);
    if (rango(p->left) < rango(p->right)) {
        NodoH* aux = p->left;
        p->left = p->right;
        p->right = aux;
    }
    p->rango = rango(p->right) + 1;
    return p;
}

struct HeapPersistente {
    vector<NodoH*> raiz;
    HeapPersistente() { raiz.push_back(nullptr); }

    int nueva(NodoH* r) {
        raiz.push_back(r);
        return (int)raiz.size() - 1;
    }

    int insertar(int v, int x) {
        NodoH* p = new NodoH;
        p->key = x; p->rango = 1; p->left = nullptr; p->right = nullptr;
        return nueva(mergeH(raiz[v], p));
    }
    int extraerMin(int v) {
        if (raiz[v] == nullptr) return nueva(nullptr);
        return nueva(mergeH(raiz[v]->left, raiz[v]->right));
    }
    int unir(int v1, int v2) { return nueva(mergeH(raiz[v1], raiz[v2])); }
    bool vacio(int v) { return raiz[v] == nullptr; }
    int minimo(int v) { return raiz[v]->key; }

    vector<int> ordenado(int v) {
        vector<int> res;
        NodoH* p = raiz[v];
        while (p != nullptr) {
            res.push_back(p->key);
            p = mergeH(p->left, p->right);
        }
        return res;
    }
    void imprimir(int v) {
        cout << "v" << v << ": ";
        vector<int> res = ordenado(v);
        for (int i = 0; i < (int)res.size(); i++) cout << res[i] << " ";
        cout << "\n";
    }
};

int main() {
    HeapPersistente H;
    int a = 0;
    int x1[4] = {7, 3, 9, 1};
    for (int i = 0; i < 4; i++) a = H.insertar(a, x1[i]);
    int b = 0;
    int x2[3] = {8, 2, 6};
    for (int i = 0; i < 3; i++) b = H.insertar(b, x2[i]);
    int c = H.extraerMin(a);
    int d = H.unir(a, b);
    int e = H.unir(d, d);  // consigo mismo

    int vs[5] = {a, b, c, d, e};
    for (int i = 0; i < 5; i++) H.imprimir(vs[i]);
    cout << "min(a)=" << H.minimo(a) << " sigue intacto tras extraerMin\n";
    return 0;
}
