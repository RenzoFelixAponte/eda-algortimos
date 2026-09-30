// segment tree persistente
// cada update copia un nodo por nivel
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

typedef long long ll;
const ll NEUTRO = 0;
ll combinar(ll a, ll b) { return a + b; }  // suma

struct NodoST {
    ll val;
    NodoST* left;
    NodoST* right;
};

struct SegTreePersistente {
    int n;
    vector<NodoST*> raiz;

    NodoST* crear(ll v, NodoST* l, NodoST* r) {
        NodoST* p = new NodoST;
        p->val = v;
        p->left = l;
        p->right = r;
        return p;
    }

    NodoST* build(const vector<ll>& a, int l, int r) {
        if (l == r) return crear(a[l], nullptr, nullptr);
        int m = (l + r) / 2;
        NodoST* n1 = build(a, l, m);
        NodoST* n2 = build(a, m + 1, r);
        return crear(combinar(n1->val, n2->val), n1, n2);
    }

    SegTreePersistente(const vector<ll>& a) {
        n = a.size();
        raiz.push_back(build(a, 0, n - 1));
    }

    NodoST* update(NodoST* p, int l, int r, int pos, ll x) {
        if (l == r) {
            return crear(x, nullptr, nullptr);
        }
        int m = (l + r) / 2;
        if (pos <= m) {
            NodoST* aux = update(p->left, l, m, pos, x);
            return crear(combinar(aux->val, p->right->val), aux, p->right);
        }
        else {
            NodoST* aux = update(p->right, m + 1, r, pos, x);
            return crear(combinar(p->left->val, aux->val), p->left, aux);
        }
    }

    ll query(NodoST* p, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) return NEUTRO;
        if (ql <= l && r <= qr) return p->val;
        int m = (l + r) / 2;
        ll n1 = query(p->left, l, m, ql, qr);
        ll n2 = query(p->right, m + 1, r, ql, qr);
        return combinar(n1, n2);
    }

    int update(int v, int pos, ll x) {
        NodoST* r = update(raiz[v], 0, n - 1, pos, x);
        raiz.push_back(r);
        return (int)raiz.size() - 1;
    }
    ll query(int v, int l, int r) { return query(raiz[v], 0, n - 1, l, r); }
    ll get(int v, int i) { return query(v, i, i); }

    void imprimir(int v) {
        cout << "v" << v << ": [";
        for (int i = 0; i < n; i++) {
            cout << get(v, i);
            if (i + 1 < n) cout << " ";
        }
        cout << "]\n";
    }
};

int main() {
    vector<ll> a = {5, 3, 8, 1, 4, 7};
    SegTreePersistente T(a);

    int v1 = T.update(0, 2, 10);
    int v2 = T.update(v1, 0, 0);
    int v3 = T.update(0, 5, 100);  // desde v0

    int vs[4] = {0, v1, v2, v3};
    for (int i = 0; i < 4; i++) T.imprimir(vs[i]);
    cout << "suma [1,4] en v0=" << T.query(0, 1, 4) << "  v1=" << T.query(v1, 1, 4)
         << "  v2=" << T.query(v2, 1, 4) << "  v3=" << T.query(v3, 1, 4) << "\n";
    cout << "comparte hijo der de la raíz v0 y v1? ";
    if (T.raiz[0]->right == T.raiz[v1]->right) cout << "si\n";
    else cout << "no\n";
    return 0;
}
