// aplicaciones de segment tree persistente (una version por prefijo)
// distintos en [l,r], k-esimo menor, cuantos <= x
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

struct NodoC {
    int cnt;
    NodoC* left;
    NodoC* right;
};

// nodo vacio que se apunta a si mismo, asi no hay que hacer build
NodoC* NULO = nullptr;
void initNulo() {
    NULO = new NodoC;
    NULO->cnt = 0;
    NULO->left = NULO;
    NULO->right = NULO;
}

NodoC* sumar(NodoC* p, int l, int r, int pos, int d) {
    NodoC* q = new NodoC;
    q->cnt = p->cnt + d;
    q->left = p->left;
    q->right = p->right;
    if (l == r) return q;
    int m = (l + r) / 2;
    if (pos <= m) q->left = sumar(p->left, l, m, pos, d);
    else q->right = sumar(p->right, m + 1, r, pos, d);
    return q;
}

int suma(NodoC* p, int l, int r, int ql, int qr) {
    if (p == NULO) return 0;
    if (qr < l || r < ql) return 0;
    if (ql <= l && r <= qr) return p->cnt;
    int m = (l + r) / 2;
    return suma(p->left, l, m, ql, qr) + suma(p->right, m + 1, r, ql, qr);
}

// distintos
struct Distintos {
    int n;
    vector<NodoC*> raiz;
    Distintos(const vector<int>& a) {
        n = a.size();
        raiz.push_back(NULO);
        map<int, int> ult;  // ultima posicion de cada valor
        for (int i = 0; i < n; i++) {
            NodoC* p = raiz.back();
            if (ult.count(a[i]) > 0) {
                p = sumar(p, 0, n - 1, ult[a[i]], -1);
            }
            p = sumar(p, 0, n - 1, i, 1);
            ult[a[i]] = i;
            raiz.push_back(p);
        }
    }
    int consulta(int l, int r) {
        return suma(raiz[r + 1], 0, n - 1, l, r);
    }
};

// kesimo y contar
struct KEsimo {
    int n, m;
    vector<int> vals;
    vector<NodoC*> raiz;
    KEsimo(const vector<int>& a) {
        n = a.size();
        vals = a;
        sort(vals.begin(), vals.end());
        vals.erase(unique(vals.begin(), vals.end()), vals.end());
        m = vals.size();
        raiz.push_back(NULO);
        for (int i = 0; i < n; i++) {
            int id = lower_bound(vals.begin(), vals.end(), a[i]) - vals.begin();
            NodoC* p = sumar(raiz.back(), 0, m - 1, id, 1);
            raiz.push_back(p);
        }
    }
    int kesimo(int l, int r, int k) {
        NodoC* p = raiz[r + 1];
        NodoC* q = raiz[l];
        int lo = 0, hi = m - 1;
        while (lo < hi) {
            int mid = (lo + hi) / 2;
            int aux = p->left->cnt - q->left->cnt;
            if (k <= aux) {
                p = p->left;
                q = q->left;
                hi = mid;
            } else {
                k = k - aux;
                p = p->right;
                q = q->right;
                lo = mid + 1;
            }
        }
        return vals[lo];
    }
    int contarMenoresIguales(int l, int r, int x) {
        int id = upper_bound(vals.begin(), vals.end(), x) - vals.begin() - 1;
        if (id < 0) return 0;
        int n1 = suma(raiz[r + 1], 0, m - 1, 0, id);
        int n2 = suma(raiz[l], 0, m - 1, 0, id);
        return n1 - n2;
    }
};

int main() {
    initNulo();
    vector<int> a = {1, 2, 1, 3, 2, 2, 4, 1};

    Distintos D(a);
    cout << "distintos [0,7]=" << D.consulta(0, 7) << "  [1,3]=" << D.consulta(1, 3)
         << "  [4,5]=" << D.consulta(4, 5) << "  [2,6]=" << D.consulta(2, 6) << "\n";

    KEsimo K(a);
    cout << "[1,5] ordenado = 1 2 2 2 3 -> k=1:" << K.kesimo(1, 5, 1) << " k=3:" << K.kesimo(1, 5, 3)
         << " k=5:" << K.kesimo(1, 5, 5) << "\n";
    cout << "cuántos <= 2 en [3,7] = " << K.contarMenoresIguales(3, 7, 2) << "\n";
    return 0;
}
