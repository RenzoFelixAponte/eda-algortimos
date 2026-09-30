// BST persistente con path copying
// se copian solo los nodos del camino, lo demas se comparte
#include <iostream>
#include <vector>
#include <set>
#include <cstdlib>
using namespace std;

long long cont = 0;   // nodos creados

template <typename K>
struct Nodo {
    K key;
    Nodo* left;
    Nodo* right;
    int tam;
};

template <typename K>
int tam(Nodo<K>* n) {
    if (n == nullptr) return 0;
    return n->tam;
}

template <typename K>
Nodo<K>* crear(K key, Nodo<K>* l, Nodo<K>* r) {
    cont++;
    Nodo<K>* p = new Nodo<K>;
    p->key = key;
    p->left = l;
    p->right = r;
    p->tam = 1 + tam(l) + tam(r);
    return p;
}

template <typename K>
struct BSTPersistente {
    vector<Nodo<K>*> raiz;

    BSTPersistente() { raiz.push_back(nullptr); }

    int nuevaVersion(Nodo<K>* r) {
        raiz.push_back(r);
        return (int)raiz.size() - 1;
    }

    Nodo<K>* insertarRec(Nodo<K>* p, K x) {
        if (p == nullptr) return crear<K>(x, nullptr, nullptr);
        if (x == p->key) return p;   // ya esta
        if (x < p->key) {
            Nodo<K>* aux = insertarRec(p->left, x);
            if (aux == p->left) return p;
            return crear<K>(p->key, aux, p->right);
        }
        else {
            Nodo<K>* aux = insertarRec(p->right, x);
            if (aux == p->right) return p;
            return crear<K>(p->key, p->left, aux);
        }
    }
    int insertar(int v, K x) {
        Nodo<K>* r = insertarRec(raiz[v], x);
        return nuevaVersion(r);
    }

    Nodo<K>* eliminarRec(Nodo<K>* p, K x) {
        if (p == nullptr) return nullptr;
        if (x < p->key) {
            Nodo<K>* aux = eliminarRec(p->left, x);
            if (aux == p->left) return p;
            return crear<K>(p->key, aux, p->right);
        }
        if (p->key < x) {
            Nodo<K>* aux = eliminarRec(p->right, x);
            if (aux == p->right) return p;
            return crear<K>(p->key, p->left, aux);
        }
        // lo encontre
        if (p->left == nullptr) return p->right;
        if (p->right == nullptr) return p->left;
        // 2 hijos, busco el sucesor
        Nodo<K>* q = p->right;
        while (q->left != nullptr) {
            q = q->left;
        }
        Nodo<K>* nuevoDer = eliminarRec(p->right, q->key);
        return crear<K>(q->key, p->left, nuevoDer);
    }
    int eliminar(int v, K x) { return nuevaVersion(eliminarRec(raiz[v], x)); }

    bool buscar(int v, K x) {
        Nodo<K>* p = raiz[v];
        while (p != nullptr) {
            if (x == p->key) return true;
            if (x < p->key) p = p->left;
            else p = p->right;
        }
        return false;
    }

    // k-esimo menor, k empieza en 1
    K kesimo(int v, int k) {
        Nodo<K>* p = raiz[v];
        while (true) {
            int n1 = tam(p->left);
            if (k == n1 + 1) return p->key;
            if (k <= n1) {
                p = p->left;
            } else {
                k = k - (n1 + 1);
                p = p->right;
            }
        }
    }

    int contarMenores(int v, K x) {
        int c = 0;
        Nodo<K>* p = raiz[v];
        while (p != nullptr) {
            if (p->key < x) {
                c = c + tam(p->left) + 1;
                p = p->right;
            }
            else p = p->left;
        }
        return c;
    }

    void inordenRec(Nodo<K>* p, vector<K>& res) {
        if (p == nullptr) return;
        inordenRec(p->left, res);
        res.push_back(p->key);
        inordenRec(p->right, res);
    }
    vector<K> inorden(int v) {
        vector<K> res;
        inordenRec(raiz[v], res);
        return res;
    }

    void imprimir(int v) {
        cout << "v" << v << ": ";
        vector<K> res = inorden(v);
        for (int i = 0; i < (int)res.size(); i++) cout << res[i] << " ";
        cout << "\n";
    }
};

int main() {
    BSTPersistente<int> T;

    int v1 = T.insertar(0, 50);
    int v2 = T.insertar(v1, 30);
    int v3 = T.insertar(v2, 70);
    int v4 = T.insertar(v3, 20);
    long long antes = cont;
    int v5 = T.insertar(v4, 40);
    cout << "insertar 40 copió " << cont - antes << " nodos (camino 50 -> 30 -> 40)\n";
    int v6 = T.eliminar(v5, 30);
    int v7 = T.insertar(v2, 10);  // desde v2

    int vs[7] = {v1, v2, v3, v4, v5, v6, v7};
    for (int i = 0; i < 7; i++) T.imprimir(vs[i]);
    cout << "v6 buscar(30)=" << T.buscar(v6, 30) << "  v5 buscar(30)=" << T.buscar(v5, 30) << "\n";
    cout << "v5: 2do menor=" << T.kesimo(v5, 2) << "  menores que 45=" << T.contarMenores(v5, 45) << "\n";
    return 0;
}
