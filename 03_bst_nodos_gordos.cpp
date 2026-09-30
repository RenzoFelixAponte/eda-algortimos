// BST parcialmente persistente con nodos gordos
// cada nodo tiene un log de 2p cambios, si se llena se hace split
// en BST p = 1
#include <iostream>
#include <vector>
#include <set>
#include <climits>
#include <cstdlib>
using namespace std;

typedef int Dato;

const int P = 1;
const int MAX_LOG = 2 * P;
const int INF_T = INT_MAX;

enum Campo { LLAVE, IZQ, DER };

struct NodoG;

struct Mod {
    Campo campo;
    Dato key;
    NodoG* ptr;
    int t;
};

struct NodoG {
    Dato key;
    NodoG* left;
    NodoG* right;
    Mod log[MAX_LOG];
    int nlog;
    NodoG* padre;  // para el split
};

long long totalSplits = 0;

struct BSTGordo {
    vector<NodoG*> raiz;
    int actual = 0;

    BSTGordo() { raiz.push_back(nullptr); }

    NodoG* leerPtr(NodoG* p, Campo c, int t) {
        for (int i = p->nlog - 1; i >= 0; i--) {
            if (p->log[i].campo == c && p->log[i].t <= t)
                return p->log[i].ptr;
        }
        if (c == IZQ) return p->left;
        return p->right;
    }
    Dato leerLlave(NodoG* p, int t) {
        for (int i = p->nlog - 1; i >= 0; i--)
            if (p->log[i].campo == LLAVE && p->log[i].t <= t) return p->log[i].key;
        return p->key;
    }

    NodoG* nuevoNodo(Dato x, NodoG* pa) {
        NodoG* p = new NodoG;
        p->key = x;
        p->left = nullptr;
        p->right = nullptr;
        p->nlog = 0;
        p->padre = pa;
        return p;
    }

    NodoG* escribir(NodoG* p, Campo c, Dato k, NodoG* q, int t) {
        if (p->nlog < MAX_LOG) {
            Mod m;
            m.campo = c; m.key = k; m.ptr = q; m.t = t;
            p->log[p->nlog] = m;
            p->nlog++;
        } else {
            // split
            totalSplits++;
            NodoG* aux = new NodoG;
            aux->key = leerLlave(p, INF_T);
            aux->left = leerPtr(p, IZQ, INF_T);
            aux->right = leerPtr(p, DER, INF_T);
            aux->nlog = 0;
            aux->padre = p->padre;
            if (c == LLAVE) aux->key = k;
            else if (c == IZQ) aux->left = q;
            else aux->right = q;

            if (aux->left != nullptr) aux->left->padre = aux;
            if (aux->right != nullptr) aux->right->padre = aux;

            if (aux->padre == nullptr) {
                raiz[t] = aux;  // era raiz
            } else {
                NodoG* pa = aux->padre;
                Campo lado;
                if (leerPtr(pa, IZQ, INF_T) == p) lado = IZQ;
                else lado = DER;
                escribir(pa, lado, 0, aux, t);  // cascada
            }
            p = aux;
        }
        if (c != LLAVE && q != nullptr) q->padre = p;
        return p;
    }

    void reemplazar(NodoG* pa, Campo lado, NodoG* h, int t) {
        if (pa == nullptr) {
            raiz[t] = h;
            if (h) h->padre = nullptr;
        }
        else {
            escribir(pa, lado, 0, h, t);
        }
    }

    Campo ladoDe(NodoG* h) {
        if (leerPtr(h->padre, IZQ, INF_T) == h) return IZQ;
        return DER;
    }

    int nuevaVersion() {
        actual++;
        raiz.push_back(raiz[actual - 1]);
        return actual;
    }

    int insertar(Dato x) {
        int t = nuevaVersion();
        if (raiz[t] == nullptr) {
            raiz[t] = nuevoNodo(x, nullptr);
            return t;
        }
        NodoG* p = raiz[t];
        while (true) {
            Dato k = leerLlave(p, t);
            if (x == k) return t;
            Campo dir;
            if (x < k) dir = IZQ; else dir = DER;
            NodoG* q = leerPtr(p, dir, t);
            if (q == nullptr) {
                escribir(p, dir, 0, nuevoNodo(x, p), t);
                return t;
            }
            p = q;
        }
    }

    int eliminar(Dato x) {
        int t = nuevaVersion();
        NodoG* z = raiz[t];
        while (z != nullptr && leerLlave(z, t) != x) {
            if (x < leerLlave(z, t)) z = leerPtr(z, IZQ, t);
            else z = leerPtr(z, DER, t);
        }
        if (z == nullptr) return t;

        NodoG* n1 = leerPtr(z, IZQ, t);
        NodoG* n2 = leerPtr(z, DER, t);
        if (n1 == nullptr || n2 == nullptr) {
            NodoG* h;
            if (n1 != nullptr) h = n1; else h = n2;
            if (z->padre == nullptr) reemplazar(nullptr, IZQ, h, t);
            else reemplazar(z->padre, ladoDe(z), h, t);
            return t;
        }
        // 2 hijos -> sucesor
        NodoG* s = n2;
        while (leerPtr(s, IZQ, t) != nullptr)
            s = leerPtr(s, IZQ, t);
        Dato ks = leerLlave(s, t);
        escribir(z, LLAVE, ks, nullptr, t);
        reemplazar(s->padre, ladoDe(s), leerPtr(s, DER, t), t);
        return t;
    }

    bool buscar(int t, Dato x) {
        NodoG* p = raiz[t];
        while (p != nullptr) {
            Dato k = leerLlave(p, t);
            if (x == k) return true;
            if (x < k) p = leerPtr(p, IZQ, t);
            else p = leerPtr(p, DER, t);
        }
        return false;
    }

    void inordenRec(NodoG* p, int t, vector<Dato>& res) {
        if (p == nullptr) return;
        inordenRec(leerPtr(p, IZQ, t), t, res);
        res.push_back(leerLlave(p, t));
        inordenRec(leerPtr(p, DER, t), t, res);
    }
    vector<Dato> inorden(int t) {
        vector<Dato> res;
        inordenRec(raiz[t], t, res);
        return res;
    }

    void imprimir(int t) {
        cout << "v" << t << ": ";
        vector<Dato> res = inorden(t);
        for (int i = 0; i < (int)res.size(); i++) cout << res[i] << " ";
        cout << "\n";
    }
};

int main() {
    BSTGordo T;
    int a[7] = {50, 30, 70, 20, 40, 60, 80};
    for (int i = 0; i < 7; i++) T.insertar(a[i]);
    T.eliminar(30);
    T.insertar(35);
    T.eliminar(50);  // la raiz

    for (int t = 0; t <= T.actual; t++) T.imprimir(t);
    cout << "splits hasta ahora: " << totalSplits << "\n";
    cout << "buscar(30) en v7=" << T.buscar(7, 30) << "  en v8=" << T.buscar(8, 30) << "\n";
    return 0;
}
