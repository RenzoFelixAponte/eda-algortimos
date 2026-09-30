// BST persistente con path copying
// solo se copian los nodos del camino raiz -> cambio, lo demas se comparte
// raiz[v] = raiz de la version v. insertar/eliminar O(h)
#include <iostream>
#include <vector>
#include <set>
#include <cstdlib>
using namespace std;

long long nodosCreados = 0;   // para ver cuántos nodos copia cada operación

template <typename K>
struct Nodo {
    K llave;
    Nodo* izq;
    Nodo* der;
    int tam;
};

template <typename K>
int tam(Nodo<K>* n) { return n ? n->tam : 0; }

template <typename K>
Nodo<K>* crear(K llave, Nodo<K>* izq, Nodo<K>* der) {
    nodosCreados++;
    return new Nodo<K>{llave, izq, der, 1 + tam(izq) + tam(der)};
}

template <typename K>
struct BSTPersistente {
    vector<Nodo<K>*> raiz;

    BSTPersistente() { raiz.push_back(nullptr); }   // versión 0 = vacío

    int nuevaVersion(Nodo<K>* r) {
        raiz.push_back(r);
        return (int)raiz.size() - 1;
    }

    // ---------- insertar ----------
    Nodo<K>* insertarRec(Nodo<K>* n, K x) {
        if (n == nullptr) return crear<K>(x, nullptr, nullptr);
        if (x == n->llave) return n;                       // ya existe: nada cambia
        if (x < n->llave) {
            Nodo<K>* h = insertarRec(n->izq, x);
            if (h == n->izq) return n;
            return crear<K>(n->llave, h, n->der);          // copia del nodo del camino
        } else {
            Nodo<K>* h = insertarRec(n->der, x);
            if (h == n->der) return n;
            return crear<K>(n->llave, n->izq, h);
        }
    }
    int insertar(int v, K x) { return nuevaVersion(insertarRec(raiz[v], x)); }

    // ---------- eliminar ----------
    Nodo<K>* eliminarRec(Nodo<K>* n, K x) {
        if (n == nullptr) return nullptr;                  // no estaba
        if (x < n->llave) {
            Nodo<K>* h = eliminarRec(n->izq, x);
            if (h == n->izq) return n;
            return crear<K>(n->llave, h, n->der);
        }
        if (n->llave < x) {
            Nodo<K>* h = eliminarRec(n->der, x);
            if (h == n->der) return n;
            return crear<K>(n->llave, n->izq, h);
        }
        // x == n->llave
        if (n->izq == nullptr) return n->der;              // se comparte el hijo
        if (n->der == nullptr) return n->izq;
        Nodo<K>* s = n->der;                               // sucesor = mínimo de la derecha
        while (s->izq != nullptr) s = s->izq;
        return crear<K>(s->llave, n->izq, eliminarRec(n->der, s->llave));
    }
    int eliminar(int v, K x) { return nuevaVersion(eliminarRec(raiz[v], x)); }

    // ---------- consultas (no crean versión) ----------
    bool buscar(int v, K x) {
        Nodo<K>* n = raiz[v];
        while (n != nullptr) {
            if (x == n->llave) return true;
            n = (x < n->llave) ? n->izq : n->der;
        }
        return false;
    }

    // k-ésimo menor (k desde 1)
    K kesimo(int v, int k) {
        Nodo<K>* n = raiz[v];
        while (true) {
            int ti = tam(n->izq);
            if (k == ti + 1) return n->llave;
            if (k <= ti) n = n->izq;
            else { k -= ti + 1; n = n->der; }
        }
    }

    // cuántas llaves < x
    int contarMenores(int v, K x) {
        int c = 0;
        Nodo<K>* n = raiz[v];
        while (n != nullptr) {
            if (n->llave < x) { c += tam(n->izq) + 1; n = n->der; }
            else n = n->izq;
        }
        return c;
    }

    void inordenRec(Nodo<K>* n, vector<K>& out) {
        if (!n) return;
        inordenRec(n->izq, out);
        out.push_back(n->llave);
        inordenRec(n->der, out);
    }
    vector<K> inorden(int v) { vector<K> out; inordenRec(raiz[v], out); return out; }

    void imprimir(int v) {
        cout << "v" << v << ": ";
        for (K x : inorden(v)) cout << x << " ";
        cout << "\n";
    }
};

int main() {
    BSTPersistente<int> T;

    int v1 = T.insertar(0, 50);
    int v2 = T.insertar(v1, 30);
    int v3 = T.insertar(v2, 70);
    int v4 = T.insertar(v3, 20);
    long long antes = nodosCreados;
    int v5 = T.insertar(v4, 40);
    cout << "insertar 40 copió " << nodosCreados - antes << " nodos (camino 50 -> 30 -> 40)\n";
    int v6 = T.eliminar(v5, 30);        // caso 2 hijos
    int v7 = T.insertar(v2, 10);        // rama desde v2 => persistencia total

    for (int v : {v1, v2, v3, v4, v5, v6, v7}) T.imprimir(v);
    cout << "v6 buscar(30)=" << T.buscar(v6, 30) << "  v5 buscar(30)=" << T.buscar(v5, 30) << "\n";
    cout << "v5: 2do menor=" << T.kesimo(v5, 2) << "  menores que 45=" << T.contarMenores(v5, 45) << "\n";
    return 0;
}
