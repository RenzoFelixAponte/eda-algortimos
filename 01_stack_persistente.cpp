// stack persistente
// cada version guarda el tope, los nodos no se tocan nunca
#include <iostream>
#include <vector>
using namespace std;

template <typename T>
struct NodoS {
    T dato;
    NodoS* sig;
};

template <typename T>
struct StackPersistente {
    vector<NodoS<T>*> ver;   // topes de cada version
    vector<int> tam;

    StackPersistente() {
        ver.push_back(nullptr);  // version 0 vacia
        tam.push_back(0);
    }

    int nuevaVersion(NodoS<T>* p, int t) {
        ver.push_back(p);
        tam.push_back(t);
        int n = ver.size();
        return n - 1;
    }

    int push(int v, T x) {
        NodoS<T>* aux = new NodoS<T>;
        aux->dato = x;
        aux->sig = ver[v];
        return nuevaVersion(aux, tam[v] + 1);
    }

    int pop(int v) {
        if (ver[v] == nullptr) {
            return nuevaVersion(nullptr, 0);
        }
        else {
            NodoS<T>* p = ver[v];
            return nuevaVersion(p->sig, tam[v] - 1);
        }
    }

    bool vacio(int v) {
        if (ver[v] == nullptr) return true;
        return false;
    }
    T top(int v) { return ver[v]->dato; }

    // concat: copia a y lo pone encima de b
    int concat(int a, int b) {
        vector<T> vals;
        NodoS<T>* p = ver[a];
        while (p != nullptr) {
            vals.push_back(p->dato);
            p = p->sig;
        }
        NodoS<T>* q = ver[b];
        int n = vals.size();
        for (int i = n - 1; i >= 0; i--) {
            NodoS<T>* aux = new NodoS<T>;
            aux->dato = vals[i];
            aux->sig = q;
            q = aux;
        }
        return nuevaVersion(q, tam[a] + tam[b]);
    }

    void imprimir(int v) {
        cout << "v" << v << ": [";
        NodoS<T>* p = ver[v];
        while (p != nullptr) {
            cout << p->dato;
            if (p->sig != nullptr) cout << " -> ";
            p = p->sig;
        }
        cout << "]  (tam " << tam[v] << ")\n";
    }
};

int main() {
    StackPersistente<int> S;

    int v1 = S.push(0, 5);
    int v2 = S.push(v1, 7);
    int v3 = S.push(v1, 9);  // desde v1 -> total
    int v4 = S.pop(v2);
  int v5 = S.concat(v2, v3); // confluente

    int vs[6] = {0, v1, v2, v3, v4, v5};
    for (int i = 0; i < 6; i++) S.imprimir(vs[i]);

    bool comp = false;
    if (S.ver[v1] == S.ver[v2]->sig && S.ver[v1] == S.ver[v3]->sig) comp = true;
    cout << "\nnodo 5 compartido entre v1, v2, v3? ";
    if (comp) cout << "si"; else cout << "no";
    cout << "\n";
    return 0;
}
