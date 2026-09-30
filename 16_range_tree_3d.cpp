// range tree 3D: arbol por x y cada nodo tiene un layered range tree en (y,z)
#define SIN_MAIN
#include "15_layered_range_tree.cpp"
#undef SIN_MAIN

struct Punto3 { int x, y, z, id; };

struct Nodo3 {
    Punto3 p;
    Nodo3* left;
    Nodo3* right;
    LayeredRangeTree* sec;
};

bool menorX3(const Punto3& a, const Punto3& b) { return a.x < b.x; }

struct RangeTree3D {
    Nodo3* raiz = nullptr;
    vector<Punto3> todos;

    Nodo3* build(vector<Punto3>& a, int l, int r) {
        if (l > r) return nullptr;
        int m = (l + r) / 2;
        Nodo3* q = new Nodo3;
        q->p = a[m];
        q->left = build(a, l, m - 1);
        q->right = build(a, m + 1, r);
        vector<Punto> yz;
        for (int i = l; i <= r; i++) {
            Punto aux = {a[i].y, a[i].z, a[i].id};
            yz.push_back(aux);
        }
        q->sec = new LayeredRangeTree(yz);
        return q;
    }
    RangeTree3D(vector<Punto3> pts) {
        todos = pts;
        sort(pts.begin(), pts.end(), menorX3);
        raiz = build(pts, 0, (int)pts.size() - 1);
    }

    bool dentro(const Punto3& p, int x1, int x2, int y1, int y2, int z1, int z2) {
        if (p.x < x1 || p.x > x2) return false;
        if (p.y < y1 || p.y > y2) return false;
        if (p.z < z1 || p.z > z2) return false;
        return true;
    }

    void canon(Nodo3* q, int y1, int y2, int z1, int z2, vector<int>& res) {
        if (q == nullptr) return;
        vector<Punto> aux = q->sec->reportar(y1, y2, z1, z2);
        for (int i = 0; i < (int)aux.size(); i++) res.push_back(aux[i].id);
    }

    vector<int> reportar(int x1, int x2, int y1, int y2, int z1, int z2) {
        vector<int> res;
        Nodo3* s = raiz;
        while (s != nullptr && (x2 < s->p.x || s->p.x < x1)) {
            if (x2 < s->p.x) s = s->left;
            else s = s->right;
        }
        if (s == nullptr) return res;
        if (dentro(s->p, x1, x2, y1, y2, z1, z2)) res.push_back(s->p.id);
        Nodo3* v = s->left;
        while (v) {
            if (x1 <= v->p.x) {
                if (dentro(v->p, x1, x2, y1, y2, z1, z2)) res.push_back(v->p.id);
                canon(v->right, y1, y2, z1, z2, res);
                v = v->left;
            }
            else v = v->right;
        }
        v = s->right;
        while (v) {
            if (v->p.x <= x2) {
                if (dentro(v->p, x1, x2, y1, y2, z1, z2)) res.push_back(v->p.id);
                canon(v->left, y1, y2, z1, z2, res);
                v = v->right;
            }
            else v = v->left;
        }
        return res;
    }
};

int main() {
    vector<Punto3> pts = {{1, 2, 3, 0}, {4, 5, 6, 1}, {2, 8, 1, 2}, {7, 3, 9, 3}, {5, 5, 5, 4}, {3, 1, 4, 5}};
    RangeTree3D T(pts);
    cout << "caja [2,6]x[1,6]x[3,6]: ids ";
    vector<int> res = T.reportar(2, 6, 1, 6, 3, 6);
    for (int i = 0; i < (int)res.size(); i++) cout << res[i] << " ";
    cout << "\n";
    return 0;
}
