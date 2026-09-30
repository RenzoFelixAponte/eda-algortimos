// range tree 2D
// BST por x, cada nodo guarda los puntos de su subarbol ordenados por y
// consulta O(lg^2 n + k), espacio O(n lg n)
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
using namespace std;

struct Punto { int x, y, id; };

struct NodoRT {
    Punto p;                 // punto guardado en el nodo (mediana por x)
    NodoRT* izq;
    NodoRT* der;
    vector<Punto> porY;      // puntos del subárbol (incluye p) ordenados por y
};

bool menorY(const Punto& a, const Punto& b) { return a.y < b.y || (a.y == b.y && a.id < b.id); }

struct RangeTree2D {
    NodoRT* raiz = nullptr;

    // a ordenado por x; devuelve el nodo y deja en "salida" sus puntos ordenados por y
    NodoRT* build(vector<Punto>& a, int l, int r) {
        if (l > r) return nullptr;
        int m = (l + r) / 2;
        NodoRT* n = new NodoRT;
        n->p = a[m];
        n->izq = build(a, l, m - 1);
        n->der = build(a, m + 1, r);
        // merge de los hijos + el propio punto (como mergesort) -> O(n lg n) total
        vector<Punto> L = n->izq ? n->izq->porY : vector<Punto>();
        vector<Punto> R = n->der ? n->der->porY : vector<Punto>();
        n->porY.resize(L.size() + R.size());
        merge(L.begin(), L.end(), R.begin(), R.end(), n->porY.begin(), menorY);
        n->porY.insert(upper_bound(n->porY.begin(), n->porY.end(), n->p, menorY), n->p);
        return n;
    }
    RangeTree2D(vector<Punto> pts) {
        sort(pts.begin(), pts.end(), [](const Punto& a, const Punto& b) { return a.x < b.x; });
        raiz = build(pts, 0, (int)pts.size() - 1);
    }

    NodoRT* split(int x1, int x2) {
        NodoRT* v = raiz;
        while (v && (x2 < v->p.x || v->p.x < x1)) v = (x2 < v->p.x) ? v->izq : v->der;
        return v;
    }

    // ---- secundaria: búsqueda 1D por y en un subárbol completo ----
    void reportarY(NodoRT* n, int y1, int y2, vector<Punto>& out) {
        if (!n) return;
        auto it = lower_bound(n->porY.begin(), n->porY.end(), Punto{0, y1, -1}, menorY);
        for (; it != n->porY.end() && it->y <= y2; ++it) out.push_back(*it);
    }
    int contarY(NodoRT* n, int y1, int y2) {
        if (!n) return 0;
        auto a = lower_bound(n->porY.begin(), n->porY.end(), Punto{0, y1, -1}, menorY);
        auto b = upper_bound(n->porY.begin(), n->porY.end(), Punto{0, y2, 1 << 30}, menorY);
        return (int)(b - a);
    }
    void revisarPunto(const Punto& p, int x1, int x2, int y1, int y2, vector<Punto>& out) {
        if (x1 <= p.x && p.x <= x2 && y1 <= p.y && p.y <= y2) out.push_back(p);
    }

    vector<Punto> reportar(int x1, int x2, int y1, int y2) {
        vector<Punto> out;
        NodoRT* s = split(x1, x2);
        if (!s) return out;
        revisarPunto(s->p, x1, x2, y1, y2, out);
        for (NodoRT* v = s->izq; v;) {                     // camino a x1
            if (x1 <= v->p.x) {
                revisarPunto(v->p, x1, x2, y1, y2, out);
                reportarY(v->der, y1, y2, out);           // subárbol canónico
                v = v->izq;
            } else v = v->der;
        }
        for (NodoRT* v = s->der; v;) {                     // camino a x2
            if (v->p.x <= x2) {
                revisarPunto(v->p, x1, x2, y1, y2, out);
                reportarY(v->izq, y1, y2, out);
                v = v->der;
            } else v = v->izq;
        }
        return out;
    }

    int contar(int x1, int x2, int y1, int y2) {
        NodoRT* s = split(x1, x2);
        if (!s) return 0;
        int c = 0;
        auto dentro = [&](const Punto& p) { return x1 <= p.x && p.x <= x2 && y1 <= p.y && p.y <= y2; };
        c += dentro(s->p);
        for (NodoRT* v = s->izq; v;) {
            if (x1 <= v->p.x) { c += dentro(v->p) + contarY(v->der, y1, y2); v = v->izq; } else v = v->der;
        }
        for (NodoRT* v = s->der; v;) {
            if (v->p.x <= x2) { c += dentro(v->p) + contarY(v->izq, y1, y2); v = v->der; } else v = v->izq;
        }
        return c;
    }
};

int main() {
    vector<Punto> pts = {{2, 7, 0}, {5, 3, 1}, {4, 9, 2}, {8, 5, 3}, {6, 6, 4}, {1, 1, 5}, {9, 8, 6}};
    RangeTree2D T(pts);
    cout << "[3,8] x [4,8]: ";
    for (auto& p : T.reportar(3, 8, 4, 8)) cout << "(" << p.x << "," << p.y << ") ";
    cout << " conteo=" << T.contar(3, 8, 4, 8) << "\n";
    return 0;
}
