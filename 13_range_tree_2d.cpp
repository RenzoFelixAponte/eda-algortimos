// range tree 2D
// arbol por x, cada nodo tiene sus puntos ordenados por y
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
using namespace std;

struct Punto { int x, y, id; };

struct NodoRT {
    Punto p;
    NodoRT* left;
    NodoRT* right;
    vector<Punto> porY;
};

bool menorY(const Punto& a, const Punto& b) {
    if (a.y < b.y) return true;
    if (a.y == b.y && a.id < b.id) return true;
    return false;
}

bool menorX(const Punto& a, const Punto& b) { return a.x < b.x; }

struct RangeTree2D {
    NodoRT* raiz = nullptr;

    NodoRT* build(vector<Punto>& a, int l, int r) {
        if (l > r) return nullptr;
        int m = (l + r) / 2;
        NodoRT* q = new NodoRT;
        q->p = a[m];
        q->left = build(a, l, m - 1);
        q->right = build(a, m + 1, r);
        // merge de los hijos como en mergesort
        vector<Punto> n1, n2;
        if (q->left != nullptr) n1 = q->left->porY;
        if (q->right != nullptr) n2 = q->right->porY;
        q->porY.resize(n1.size() + n2.size());
        merge(n1.begin(), n1.end(), n2.begin(), n2.end(), q->porY.begin(), menorY);
        q->porY.insert(upper_bound(q->porY.begin(), q->porY.end(), q->p, menorY), q->p);
        return q;
    }
    RangeTree2D(vector<Punto> pts) {
        sort(pts.begin(), pts.end(), menorX);
        raiz = build(pts, 0, (int)pts.size() - 1);
    }

    NodoRT* split(int x1, int x2) {
        NodoRT* v = raiz;
        while (v != nullptr && (x2 < v->p.x || v->p.x < x1)) {
            if (x2 < v->p.x) v = v->left;
            else v = v->right;
        }
        return v;
    }

    // busqueda en y
    void reportarY(NodoRT* q, int y1, int y2, vector<Punto>& res) {
        if (q == nullptr) return;
        Punto aux = {0, y1, -1};
        int i = lower_bound(q->porY.begin(), q->porY.end(), aux, menorY) - q->porY.begin();
        while (i < (int)q->porY.size() && q->porY[i].y <= y2) {
            res.push_back(q->porY[i]);
            i++;
        }
    }
    int contarY(NodoRT* q, int y1, int y2) {
        if (q == nullptr) return 0;
        Punto p1 = {0, y1, -1};
        Punto p2 = {0, y2, 1 << 30};
        int a = lower_bound(q->porY.begin(), q->porY.end(), p1, menorY) - q->porY.begin();
        int b = upper_bound(q->porY.begin(), q->porY.end(), p2, menorY) - q->porY.begin();
        return b - a;
    }
    bool dentro(const Punto& p, int x1, int x2, int y1, int y2) {
        if (x1 <= p.x && p.x <= x2 && y1 <= p.y && p.y <= y2) return true;
        return false;
    }
    void revisarPunto(const Punto& p, int x1, int x2, int y1, int y2, vector<Punto>& res) {
        if (dentro(p, x1, x2, y1, y2)) res.push_back(p);
    }

    vector<Punto> reportar(int x1, int x2, int y1, int y2) {
        vector<Punto> res;
        NodoRT* s = split(x1, x2);
        if (s == nullptr) return res;
        revisarPunto(s->p, x1, x2, y1, y2, res);
        NodoRT* v = s->left;
        while (v != nullptr) {
            if (x1 <= v->p.x) {
                revisarPunto(v->p, x1, x2, y1, y2, res);
                reportarY(v->right, y1, y2, res);
                v = v->left;
            }
            else v = v->right;
        }
        v = s->right;
        while (v != nullptr) {
            if (v->p.x <= x2) {
                revisarPunto(v->p, x1, x2, y1, y2, res);
                reportarY(v->left, y1, y2, res);
                v = v->right;
            }
            else v = v->left;
        }
        return res;
    }

    int contar(int x1, int x2, int y1, int y2) {
        NodoRT* s = split(x1, x2);
        if (s == nullptr) return 0;
        int c = 0;
        if (dentro(s->p, x1, x2, y1, y2)) c++;
        NodoRT* v = s->left;
        while (v) {
            if (x1 <= v->p.x) {
                if (dentro(v->p, x1, x2, y1, y2)) c++;
                c += contarY(v->right, y1, y2);
                v = v->left;
            } else v = v->right;
        }
        v = s->right;
        while (v) {
            if (v->p.x <= x2) {
                if (dentro(v->p, x1, x2, y1, y2)) c++;
                c += contarY(v->left, y1, y2);
                v = v->right;
            } else v = v->left;
        }
        return c;
    }
};

int main() {
    vector<Punto> pts = {{2, 7, 0}, {5, 3, 1}, {4, 9, 2}, {8, 5, 3}, {6, 6, 4}, {1, 1, 5}, {9, 8, 6}};
    RangeTree2D T(pts);
    cout << "[3,8] x [4,8]: ";
    vector<Punto> res = T.reportar(3, 8, 4, 8);
    for (int i = 0; i < (int)res.size(); i++) cout << "(" << res[i].x << "," << res[i].y << ") ";
    cout << " conteo=" << T.contar(3, 8, 4, 8) << "\n";
    return 0;
}
