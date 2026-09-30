// aplicaciones del segment tree persistente: una version por prefijo
// (A) cantidad de distintos en [l,r]  (B) k-esimo menor en [l,r]  (C) cuantos <= x en [l,r]
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

struct NodoC {
    int cnt;
    NodoC* izq;
    NodoC* der;
};

// nodo "vacío" compartido: sus hijos se apuntan a sí mismo => no hace falta build
NodoC* NULO = nullptr;
void initNulo() {
    NULO = new NodoC{0, nullptr, nullptr};
    NULO->izq = NULO->der = NULO;
}

// suma "delta" en la posición pos (path copying)
NodoC* sumar(NodoC* nodo, int l, int r, int pos, int delta) {
    NodoC* c = new NodoC{nodo->cnt + delta, nodo->izq, nodo->der};
    if (l == r) return c;
    int m = (l + r) / 2;
    if (pos <= m) c->izq = sumar(nodo->izq, l, m, pos, delta);
    else          c->der = sumar(nodo->der, m + 1, r, pos, delta);
    return c;
}

int suma(NodoC* nodo, int l, int r, int ql, int qr) {
    if (nodo == NULO || qr < l || r < ql) return 0;
    if (ql <= l && r <= qr) return nodo->cnt;
    int m = (l + r) / 2;
    return suma(nodo->izq, l, m, ql, qr) + suma(nodo->der, m + 1, r, ql, qr);
}

// ---------------- (A) distintos en [l, r] ----------------
struct Distintos {
    int n;
    vector<NodoC*> raiz;
    Distintos(const vector<int>& a) {
        n = (int)a.size();
        raiz.push_back(NULO);
        map<int, int> ult;                       // valor -> última posición vista
        for (int i = 0; i < n; i++) {
            NodoC* r = raiz.back();
            if (ult.count(a[i])) r = sumar(r, 0, n - 1, ult[a[i]], -1);
            r = sumar(r, 0, n - 1, i, +1);
            ult[a[i]] = i;
            raiz.push_back(r);
        }
    }
    int consulta(int l, int r) { return suma(raiz[r + 1], 0, n - 1, l, r); }
};

// ---------------- (B)(C) k-ésimo menor / contar <= x ----------------
struct KEsimo {
    int n, m;
    vector<int> vals;                            // valores ordenados sin repetir
    vector<NodoC*> raiz;
    KEsimo(const vector<int>& a) {
        n = (int)a.size();
        vals = a;
        sort(vals.begin(), vals.end());
        vals.erase(unique(vals.begin(), vals.end()), vals.end());
        m = (int)vals.size();
        raiz.push_back(NULO);
        for (int i = 0; i < n; i++) {
            int id = lower_bound(vals.begin(), vals.end(), a[i]) - vals.begin();
            raiz.push_back(sumar(raiz.back(), 0, m - 1, id, +1));
        }
    }
    // k desde 1
    int kesimo(int l, int r, int k) {
        NodoC* A = raiz[r + 1];
        NodoC* B = raiz[l];
        int lo = 0, hi = m - 1;
        while (lo < hi) {
            int mid = (lo + hi) / 2;
            int enIzq = A->izq->cnt - B->izq->cnt;   // cuántos de a[l..r] caen en [lo, mid]
            if (k <= enIzq) { A = A->izq; B = B->izq; hi = mid; }
            else { k -= enIzq; A = A->der; B = B->der; lo = mid + 1; }
        }
        return vals[lo];
    }
    int contarMenoresIguales(int l, int r, int x) {
        int id = upper_bound(vals.begin(), vals.end(), x) - vals.begin() - 1;
        if (id < 0) return 0;
        return suma(raiz[r + 1], 0, m - 1, 0, id) - suma(raiz[l], 0, m - 1, 0, id);
    }
};

int main() {
    initNulo();
    vector<int> a = {1, 2, 1, 3, 2, 2, 4, 1};
    //               0  1  2  3  4  5  6  7

    Distintos D(a);
    cout << "distintos [0,7]=" << D.consulta(0, 7) << "  [1,3]=" << D.consulta(1, 3)
         << "  [4,5]=" << D.consulta(4, 5) << "  [2,6]=" << D.consulta(2, 6) << "\n";

    KEsimo K(a);
    cout << "[1,5] ordenado = 1 2 2 2 3 -> k=1:" << K.kesimo(1, 5, 1) << " k=3:" << K.kesimo(1, 5, 3)
         << " k=5:" << K.kesimo(1, 5, 5) << "\n";
    cout << "cuántos <= 2 en [3,7] = " << K.contarMenoresIguales(3, 7, 2) << "\n";
    return 0;
}
