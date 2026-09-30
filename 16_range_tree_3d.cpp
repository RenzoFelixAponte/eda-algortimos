// range tree 3D: BST por x y en cada nodo un layered range tree en (y, z)
// consulta O(lg^2 n + k), espacio O(n lg^2 n)
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
    return 0;
}
