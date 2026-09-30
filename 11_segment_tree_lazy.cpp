// segment tree con lazy propagation (suma en rango + sumar a un rango)
#include <iostream>
#include <vector>
using namespace std;

typedef long long Valor;

struct SegTreeLazy {
    int n;
    vector<Valor> t, lazy;   // t[nodo] = suma del rango, lazy[nodo] = suma pendiente para los hijos

    SegTreeLazy(const vector<Valor>& a) : n((int)a.size()), t(4 * a.size()), lazy(4 * a.size(), 0) {
        build(a, 1, 0, n - 1);
    }
    void build(const vector<Valor>& a, int nodo, int l, int r) {
        if (l == r) { t[nodo] = a[l]; return; }
        int m = (l + r) / 2;
        build(a, 2 * nodo, l, m);
        build(a, 2 * nodo + 1, m + 1, r);
        t[nodo] = t[2 * nodo] + t[2 * nodo + 1];
    }
    void aplicar(int nodo, int l, int r, Valor v) {   // sumar v a todo el rango del nodo
        t[nodo] += v * (r - l + 1);
        lazy[nodo] += v;
    }
    void empujar(int nodo, int l, int r) {
        if (lazy[nodo] == 0) return;
        int m = (l + r) / 2;
        aplicar(2 * nodo, l, m, lazy[nodo]);
        aplicar(2 * nodo + 1, m + 1, r, lazy[nodo]);
        lazy[nodo] = 0;
    }
    void sumarRango(int nodo, int l, int r, int ql, int qr, Valor v) {
        if (qr < l || r < ql) return;
        if (ql <= l && r <= qr) { aplicar(nodo, l, r, v); return; }
        empujar(nodo, l, r);
        int m = (l + r) / 2;
        sumarRango(2 * nodo, l, m, ql, qr, v);
        sumarRango(2 * nodo + 1, m + 1, r, ql, qr, v);
        t[nodo] = t[2 * nodo] + t[2 * nodo + 1];
    }
    void asignar(int nodo, int l, int r, int pos, Valor v) {
        if (l == r) { t[nodo] = v; return; }
        empujar(nodo, l, r);
        int m = (l + r) / 2;
        if (pos <= m) asignar(2 * nodo, l, m, pos, v);
        else asignar(2 * nodo + 1, m + 1, r, pos, v);
        t[nodo] = t[2 * nodo] + t[2 * nodo + 1];
    }
    Valor suma(int nodo, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) return 0;
        if (ql <= l && r <= qr) return t[nodo];
        empujar(nodo, l, r);
        int m = (l + r) / 2;
        return suma(2 * nodo, l, m, ql, qr) + suma(2 * nodo + 1, m + 1, r, ql, qr);
    }

    // interfaz
    void sumarRango(int l, int r, Valor v) { sumarRango(1, 0, n - 1, l, r, v); }
    void asignar(int pos, Valor v) { asignar(1, 0, n - 1, pos, v); }
    Valor suma(int l, int r) { return suma(1, 0, n - 1, l, r); }
};

int main() {
    vector<Valor> a = {1, 2, 3, 4, 5, 6, 7, 8};
    SegTreeLazy T(a);
    cout << "suma[0,7]=" << T.suma(0, 7) << "\n";          // 36
    T.sumarRango(2, 5, 10);                                // 1 2 13 14 15 16 7 8
    cout << "tras +10 en [2,5]: suma[0,3]=" << T.suma(0, 3) << "\n";   // 30
    T.asignar(3, 0);                                       // 1 2 13 0 15 16 7 8
    cout << "tras a[3]=0: suma[3,6]=" << T.suma(3, 6) << "\n";         // 38
    return 0;
}
