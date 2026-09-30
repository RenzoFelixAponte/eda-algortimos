// =====================================================================
// 15 - LAYERED RANGE TREE 2D  (Range Tree + Fractional Cascading) — Semana 5
// ---------------------------------------------------------------------
// Igual que el Range Tree 2D (13): BST primario por x y en cada nodo el
// arreglo de puntos del subárbol ordenado por y.
// La diferencia: cada posición j del arreglo de v guarda punteros
//   pI[j] = posición en el arreglo del hijo IZQ del primer punto >= A_v[j]
//   pD[j] = lo mismo en el hijo DER
// (se puede porque el arreglo de cada hijo es un SUBCONJUNTO del de v).
// => Solo UNA búsqueda binaria por y (en v_split); al bajar se siguen
//    punteros en O(1). "Nunca hay un árbol en y."
//
//   Espacio O(n lg n) | Construir O(n lg n)
//   Enumeración O(lg n + k) | Conteo O(lg n)   (antes O(lg² n))
//
// También resuelve DOMINANCIA: y <= b1 y z <= b2 -> rango (-INF, b1] x (-INF, b2].
// (El archivo 16 lo usa como estructura secundaria del Range Tree 3D.)
// =====================================================================
#ifndef LAYERED_RANGE_TREE
#define LAYERED_RANGE_TREE
#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <cstdlib>
using namespace std;

struct Punto { int x, y, id; };

inline bool menorY(const Punto& a, const Punto& b) { return a.y < b.y || (a.y == b.y && a.id < b.id); }

struct NodoL {
    Punto p;
    NodoL* izq;
    NodoL* der;
    vector<Punto> A;        // puntos del subárbol ordenados por y
    vector<int> pI, pD;     // tamaño |A|+1 (la última posición = "no hay")
};

struct LayeredRangeTree {
    NodoL* raiz = nullptr;

    static vector<int> punteros(const vector<Punto>& A, NodoL* hijo) {
        vector<int> p(A.size() + 1, 0);
        if (!hijo) return p;
        const vector<Punto>& B = hijo->A;
        size_t k = 0;                                   // merge lineal
        for (size_t j = 0; j < A.size(); j++) {
            while (k < B.size() && menorY(B[k], A[j])) k++;
            p[j] = (int)k;
        }
        p[A.size()] = (int)B.size();
        return p;
    }

    NodoL* build(vector<Punto>& a, int l, int r) {
        if (l > r) return nullptr;
        int m = (l + r) / 2;
        NodoL* n = new NodoL;
        n->p = a[m];
        n->izq = build(a, l, m - 1);
        n->der = build(a, m + 1, r);
        vector<Punto> L = n->izq ? n->izq->A : vector<Punto>();
        vector<Punto> R = n->der ? n->der->A : vector<Punto>();
        n->A.resize(L.size() + R.size());
        merge(L.begin(), L.end(), R.begin(), R.end(), n->A.begin(), menorY);
        n->A.insert(upper_bound(n->A.begin(), n->A.end(), n->p, menorY), n->p);
        n->pI = punteros(n->A, n->izq);
        n->pD = punteros(n->A, n->der);
        return n;
    }

    LayeredRangeTree() {}
    LayeredRangeTree(vector<Punto> pts) {
        sort(pts.begin(), pts.end(), [](const Punto& a, const Punto& b) { return a.x < b.x; });
        raiz = build(pts, 0, (int)pts.size() - 1);
    }

    NodoL* split(int x1, int x2) {
        NodoL* v = raiz;
        while (v && (x2 < v->p.x || v->p.x < x1)) v = (x2 < v->p.x) ? v->izq : v->der;
        return v;
    }

    // Recorre la consulta. Para cada subárbol canónico llama f(arreglo, desde, hasta)
    // y para cada punto suelto del camino llama g(punto).
    template <typename F, typename G>
    void recorrer(int x1, int x2, int y1, int y2, F f, G g) {
        NodoL* s = split(x1, x2);
        if (!s) return;
        // ÚNICA búsqueda binaria: dos umbrales (primer y >= y1, primer y > y2)
        int a = (int)(lower_bound(s->A.begin(), s->A.end(), Punto{0, y1, INT_MIN}, menorY) - s->A.begin());
        int b = (int)(lower_bound(s->A.begin(), s->A.end(), Punto{0, y2, INT_MAX}, menorY) - s->A.begin());
        auto dentro = [&](const Punto& p) { return x1 <= p.x && p.x <= x2 && y1 <= p.y && p.y <= y2; };
        if (dentro(s->p)) g(s->p);

        // camino a x1
        NodoL* v = s->izq; int ja = s->pI[a], jb = s->pI[b];
        while (v) {
            if (x1 <= v->p.x) {
                if (dentro(v->p)) g(v->p);
                if (v->der) f(v->der->A, v->pD[ja], v->pD[jb]);     // subárbol canónico en O(1)
                int na = v->pI[ja], nb = v->pI[jb];
                ja = na; jb = nb; v = v->izq;
            } else {
                int na = v->pD[ja], nb = v->pD[jb];
                ja = na; jb = nb; v = v->der;
            }
        }
        // camino a x2
        v = s->der; ja = s->pD[a]; jb = s->pD[b];
        while (v) {
            if (v->p.x <= x2) {
                if (dentro(v->p)) g(v->p);
                if (v->izq) f(v->izq->A, v->pI[ja], v->pI[jb]);
                int na = v->pD[ja], nb = v->pD[jb];
                ja = na; jb = nb; v = v->der;
            } else {
                int na = v->pI[ja], nb = v->pI[jb];
                ja = na; jb = nb; v = v->izq;
            }
        }
    }

    vector<Punto> reportar(int x1, int x2, int y1, int y2) {
        vector<Punto> out;
        recorrer(x1, x2, y1, y2,
                 [&](const vector<Punto>& A, int d, int h) { for (int i = d; i < h; i++) out.push_back(A[i]); },
                 [&](const Punto& p) { out.push_back(p); });
        return out;
    }
    int contar(int x1, int x2, int y1, int y2) {
        int c = 0;
        recorrer(x1, x2, y1, y2,
                 [&](const vector<Punto>&, int d, int h) { c += h - d; },
                 [&](const Punto&) { c++; });
        return c;
    }
    // dominancia: x <= bx  y  y <= by
    vector<Punto> dominados(int bx, int by) { return reportar(INT_MIN, bx, INT_MIN, by); }
};
#endif

#ifndef SIN_MAIN
int main() {
    // ejercicio de dominancia de tu Notion: puntos (y, z), consulta (6, 6)
    vector<Punto> pts = {{2, 7, 'A'}, {5, 3, 'B'}, {4, 9, 'C'}, {8, 5, 'D'}, {6, 6, 'E'}};
    LayeredRangeTree T(pts);
    cout << "dominados por (6,6): ";
    for (auto& p : T.dominados(6, 6)) cout << (char)p.id << "(" << p.x << "," << p.y << ") ";
    cout << "\n";

    // prueba aleatoria vs fuerza bruta
    srand(21);
    vector<Punto> v;
    for (int i = 0; i < 500; i++) v.push_back({rand() % 100, rand() % 100, i});
    LayeredRangeTree R(v); bool ok = true;
    for (int q = 0; q < 5000; q++) {
        int x1 = rand() % 110 - 5, x2 = rand() % 110 - 5, y1 = rand() % 110 - 5, y2 = rand() % 110 - 5;
        if (x1 > x2) swap(x1, x2);
        if (y1 > y2) swap(y1, y2);
        vector<int> a, b;
        for (auto& p : v) if (x1 <= p.x && p.x <= x2 && y1 <= p.y && p.y <= y2) a.push_back(p.id);
        for (auto& p : R.reportar(x1, x2, y1, y2)) b.push_back(p.id);
        sort(a.begin(), a.end()); sort(b.begin(), b.end());
        if (a != b || (int)a.size() != R.contar(x1, x2, y1, y2)) ok = false;
    }
    cout << "prueba aleatoria: " << (ok ? "OK" : "FALLO") << "\n";
    return 0;
}
#endif
