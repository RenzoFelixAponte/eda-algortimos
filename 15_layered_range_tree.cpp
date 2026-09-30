// layered range tree 2D (range tree + fractional cascading)
// solo se hace una busqueda binaria en el split y despues se siguen punteros
// tambien sirve para dominancia
#ifndef LAYERED_RANGE_TREE
#define LAYERED_RANGE_TREE
#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <cstdlib>
using namespace std;

struct Punto { int x, y, id; };

inline bool menorY(const Punto& a, const Punto& b) {
    if (a.y < b.y) return true;
    if (a.y == b.y && a.id < b.id) return true;
    return false;
}

inline bool menorX(const Punto& a, const Punto& b) { return a.x < b.x; }

struct NodoL {
    Punto p;
    NodoL* left;
    NodoL* right;
    vector<Punto> A;     // ordenados por y
    vector<int> pI, pD;  // punteros a los hijos, tam |A|+1
};

struct LayeredRangeTree {
    NodoL* raiz = nullptr;

    static vector<int> punteros(const vector<Punto>& A, NodoL* h) {
        vector<int> p(A.size() + 1, 0);
        if (h == nullptr) return p;
        const vector<Punto>& B = h->A;
        int k = 0;
        for (int j = 0; j < (int)A.size(); j++) {
            while (k < (int)B.size() && menorY(B[k], A[j])) k++;
            p[j] = k;
        }
        p[A.size()] = B.size();
        return p;
    }

    NodoL* build(vector<Punto>& a, int l, int r) {
        if (l > r) return nullptr;
        int m = (l + r) / 2;
        NodoL* q = new NodoL;
        q->p = a[m];
        q->left = build(a, l, m - 1);
        q->right = build(a, m + 1, r);
        vector<Punto> n1, n2;
        if (q->left) n1 = q->left->A;
        if (q->right) n2 = q->right->A;
        q->A.resize(n1.size() + n2.size());
        merge(n1.begin(), n1.end(), n2.begin(), n2.end(), q->A.begin(), menorY);
        q->A.insert(upper_bound(q->A.begin(), q->A.end(), q->p, menorY), q->p);
        q->pI = punteros(q->A, q->left);
        q->pD = punteros(q->A, q->right);
        return q;
    }

    LayeredRangeTree() {}
    LayeredRangeTree(vector<Punto> pts) {
        sort(pts.begin(), pts.end(), menorX);
        raiz = build(pts, 0, (int)pts.size() - 1);
    }

    NodoL* split(int x1, int x2) {
        NodoL* v = raiz;
        while (v != nullptr && (x2 < v->p.x || v->p.x < x1)) {
            if (x2 < v->p.x) v = v->left;
            else v = v->right;
        }
        return v;
    }

    bool dentro(const Punto& p, int x1, int x2, int y1, int y2) {
        return x1 <= p.x && p.x <= x2 && y1 <= p.y && p.y <= y2;
    }

    // si res != nullptr guarda los puntos, si c != nullptr solo cuenta
    void recorrer(int x1, int x2, int y1, int y2, vector<Punto>* res, int* c) {
        NodoL* s = split(x1, x2);
        if (s == nullptr) return;
        // la unica binaria
        Punto p1 = {0, y1, INT_MIN};
        Punto p2 = {0, y2, INT_MAX};
        int a = lower_bound(s->A.begin(), s->A.end(), p1, menorY) - s->A.begin();
        int b = lower_bound(s->A.begin(), s->A.end(), p2, menorY) - s->A.begin();
        if (dentro(s->p, x1, x2, y1, y2)) {
            if (res) res->push_back(s->p);
            if (c) (*c)++;
        }

        // hacia x1
        NodoL* v = s->left;
        int ja = s->pI[a], jb = s->pI[b];
        while (v != nullptr) {
            if (x1 <= v->p.x) {
                if (dentro(v->p, x1, x2, y1, y2)) {
                    if (res) res->push_back(v->p);
                    if (c) (*c)++;
                }
                if (v->right != nullptr) {
                    int d = v->pD[ja], hh = v->pD[jb];
                    if (res) for (int i = d; i < hh; i++) res->push_back(v->right->A[i]);
                    if (c) *c += hh - d;
                }
                int na = v->pI[ja], nb = v->pI[jb];
                ja = na; jb = nb;
                v = v->left;
            } else {
                int na = v->pD[ja], nb = v->pD[jb];
                ja = na; jb = nb;
                v = v->right;
            }
        }
        // hacia x2
        v = s->right;
        ja = s->pD[a];
        jb = s->pD[b];
        while (v != nullptr) {
            if (v->p.x <= x2) {
                if (dentro(v->p, x1, x2, y1, y2)) {
                    if (res) res->push_back(v->p);
                    if (c) (*c)++;
                }
                if (v->left != nullptr) {
                    int d = v->pI[ja], hh = v->pI[jb];
                    if (res) for (int i = d; i < hh; i++) res->push_back(v->left->A[i]);
                    if (c) *c += hh - d;
                }
                int na = v->pD[ja], nb = v->pD[jb];
                ja = na; jb = nb;
                v = v->right;
            } else {
                int na = v->pI[ja], nb = v->pI[jb];
                ja = na; jb = nb;
                v = v->left;
            }
        }
    }

    vector<Punto> reportar(int x1, int x2, int y1, int y2) {
        vector<Punto> res;
        recorrer(x1, x2, y1, y2, &res, nullptr);
        return res;
    }
    int contar(int x1, int x2, int y1, int y2) {
        int c = 0;
        recorrer(x1, x2, y1, y2, nullptr, &c);
        return c;
    }
    // dominancia
    vector<Punto> dominados(int bx, int by) { return reportar(INT_MIN, bx, INT_MIN, by); }
};
#endif

#ifndef SIN_MAIN
int main() {
    // dominancia con (6,6)
    vector<Punto> pts = {{2, 7, 'A'}, {5, 3, 'B'}, {4, 9, 'C'}, {8, 5, 'D'}, {6, 6, 'E'}};
    LayeredRangeTree T(pts);
    cout << "dominados por (6,6): ";
    vector<Punto> res = T.dominados(6, 6);
    for (int i = 0; i < (int)res.size(); i++)
        cout << (char)res[i].id << "(" << res[i].x << "," << res[i].y << ") ";
    cout << "\n";
    return 0;
}
#endif
