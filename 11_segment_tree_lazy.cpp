// segment tree con lazy (sumar en rango y suma en rango)
#include <iostream>
#include <vector>
using namespace std;

typedef long long ll;

struct SegTreeLazy {
    int n;
    vector<ll> t, lazy;  // lazy = lo que falta pasar a los hijos

    SegTreeLazy(const vector<ll>& a) {
        n = a.size();
        t.assign(4 * a.size(), 0);
        lazy.assign(4 * a.size(), 0);
        build(a, 1, 0, n - 1);
    }
    void build(const vector<ll>& a, int p, int l, int r) {
        if (l == r) {
            t[p] = a[l];
            return;
        }
        int m = (l + r) / 2;
        build(a, 2 * p, l, m);
        build(a, 2 * p + 1, m + 1, r);
        t[p] = t[2 * p] + t[2 * p + 1];
    }
    void aplicar(int p, int l, int r, ll v) {
        t[p] = t[p] + v * (r - l + 1);
        lazy[p] = lazy[p] + v;
    }
    void empujar(int p, int l, int r) {
        if (lazy[p] == 0) return;
        int m = (l + r) / 2;
        aplicar(2 * p, l, m, lazy[p]);
        aplicar(2 * p + 1, m + 1, r, lazy[p]);
        lazy[p] = 0;
    }
    void sumarRango(int p, int l, int r, int ql, int qr, ll v) {
        if (qr < l || r < ql) return;
        if (ql <= l && r <= qr) {
            aplicar(p, l, r, v);
            return;
        }
        empujar(p, l, r);
        int m = (l + r) / 2;
        sumarRango(2 * p, l, m, ql, qr, v);
        sumarRango(2 * p + 1, m + 1, r, ql, qr, v);
        t[p] = t[2 * p] + t[2 * p + 1];
    }
    void asignar(int p, int l, int r, int pos, ll v) {
        if (l == r) { t[p] = v; return; }
        empujar(p, l, r);
        int m = (l + r) / 2;
        if (pos <= m) asignar(2 * p, l, m, pos, v);
        else asignar(2 * p + 1, m + 1, r, pos, v);
        t[p] = t[2 * p] + t[2 * p + 1];
    }
    ll suma(int p, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) return 0;
        if (ql <= l && r <= qr) return t[p];
        empujar(p, l, r);
        int m = (l + r) / 2;
        ll n1 = suma(2 * p, l, m, ql, qr);
        ll n2 = suma(2 * p + 1, m + 1, r, ql, qr);
        return n1 + n2;
    }

    void sumarRango(int l, int r, ll v) { sumarRango(1, 0, n - 1, l, r, v); }
    void asignar(int pos, ll v) { asignar(1, 0, n - 1, pos, v); }
    ll suma(int l, int r) { return suma(1, 0, n - 1, l, r); }
};

int main() {
    vector<ll> a = {1, 2, 3, 4, 5, 6, 7, 8};
    SegTreeLazy T(a);
    cout << "suma[0,7]=" << T.suma(0, 7) << "\n";
    T.sumarRango(2, 5, 10);
    cout << "tras +10 en [2,5]: suma[0,3]=" << T.suma(0, 3) << "\n";
    T.asignar(3, 0);
    cout << "tras a[3]=0: suma[3,6]=" << T.suma(3, 6) << "\n";
    return 0;
}
