// =====================================================================
// 16 - RANGE TREE 3D  — Semana 5 (Rango 3D)
// ---------------------------------------------------------------------
// Caja [x1,x2] x [y1,y2] x [z1,z2].
// Árbol primario por x; cada nodo v guarda un LAYERED RANGE TREE 2D
// (archivo 15) con los puntos de su subárbol en coordenadas (y, z).
//   - buscar v_split por x, bajar por los dos caminos
//   - cada subárbol canónico (O(lg n) de ellos) -> consulta 2D en O(lg n + k_i)
//
//   Espacio O(n lg² n) | Consulta O(lg² n + k)
// (En general, d dimensiones: O(lg^{d-1} n + k) con el último nivel "layered".)
//
// En la clase: para bajar a O(lg n + k) se usa una estructura de
// dominancia especial en el último nivel (con persistencia / rayos);
// esa parte es más teórica y aquí se deja con el layered normal.
// =====================================================================
#define SIN_MAIN
#include "15_layered_range_tree.cpp"
#undef SIN_MAIN

struct Punto3 { int x, y, z, id; };

struct Nodo3 {
    Punto3 p;
    Nodo3* izq;
    Nodo3* der;
    LayeredRangeTree* sec;   // estructura secundaria sobre (y, z)
};

struct RangeTree3D {
    Nodo3* raiz = nullptr;
    vector<Punto3> todos;    // para traducir id -> punto

    Nodo3* build(vector<Punto3>& a, int l, int r) {
        if (l > r) return nullptr;
        int m = (l + r) / 2;
        Nodo3* n = new Nodo3;
        n->p = a[m];
        n->izq = build(a, l, m - 1);
        n->der = build(a, m + 1, r);
        vector<Punto> yz;
        for (int i = l; i <= r; i++) yz.push_back({a[i].y, a[i].z, a[i].id});   // (x,y) de 2D = (y,z)
        n->sec = new LayeredRangeTree(yz);
        return n;
    }
    RangeTree3D(vector<Punto3> pts) : todos(pts) {
        sort(pts.begin(), pts.end(), [](const Punto3& a, const Punto3& b) { return a.x < b.x; });
        raiz = build(pts, 0, (int)pts.size() - 1);
    }

    vector<int> reportar(int x1, int x2, int y1, int y2, int z1, int z2) {
        vector<int> out;
        auto dentro = [&](const Punto3& p) {
            return x1 <= p.x && p.x <= x2 && y1 <= p.y && p.y <= y2 && z1 <= p.z && p.z <= z2;
        };
        auto canon = [&](Nodo3* n) {
            if (!n) return;
            for (auto& q : n->sec->reportar(y1, y2, z1, z2)) out.push_back(q.id);
        };
        Nodo3* s = raiz;
        while (s && (x2 < s->p.x || s->p.x < x1)) s = (x2 < s->p.x) ? s->izq : s->der;
        if (!s) return out;
        if (dentro(s->p)) out.push_back(s->p.id);
        for (Nodo3* v = s->izq; v;) {
            if (x1 <= v->p.x) { if (dentro(v->p)) out.push_back(v->p.id); canon(v->der); v = v->izq; }
            else v = v->der;
        }
        for (Nodo3* v = s->der; v;) {
            if (v->p.x <= x2) { if (dentro(v->p)) out.push_back(v->p.id); canon(v->izq); v = v->der; }
            else v = v->izq;
        }
        return out;
    }
};

int main() {
    vector<Punto3> pts = {{1, 2, 3, 0}, {4, 5, 6, 1}, {2, 8, 1, 2}, {7, 3, 9, 3}, {5, 5, 5, 4}, {3, 1, 4, 5}};
    RangeTree3D T(pts);
    cout << "caja [2,6]x[1,6]x[3,6]: ids ";
    for (int id : T.reportar(2, 6, 1, 6, 3, 6)) cout << id << " ";
    cout << "\n";

    srand(33);
    vector<Punto3> v;
    for (int i = 0; i < 300; i++) v.push_back({rand() % 50, rand() % 50, rand() % 50, i});
    RangeTree3D R(v); bool ok = true;
    for (int q = 0; q < 2000; q++) {
        int c[6];
        for (int& t : c) t = rand() % 55 - 2;
        for (int d = 0; d < 6; d += 2) if (c[d] > c[d + 1]) swap(c[d], c[d + 1]);
        vector<int> a, b = R.reportar(c[0], c[1], c[2], c[3], c[4], c[5]);
        for (auto& p : v)
            if (c[0] <= p.x && p.x <= c[1] && c[2] <= p.y && p.y <= c[3] && c[4] <= p.z && p.z <= c[5]) a.push_back(p.id);
        sort(a.begin(), a.end()); sort(b.begin(), b.end());
        if (a != b) ok = false;
    }
    cout << "prueba aleatoria: " << (ok ? "OK" : "FALLO") << "\n";
    return 0;
}
