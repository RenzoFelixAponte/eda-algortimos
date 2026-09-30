// vista geometrica de BST, puntos (llave, tiempo)
// arboralmente satisfecho = todo rectangulo entre 2 puntos tiene otro punto
// greedy: en cada fila se toca x y la escalera
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cstdlib>
using namespace std;

typedef pair<int, int> Pt;  // (llave, tiempo)

bool satisfecho(const set<Pt>& S) {
    vector<Pt> v(S.begin(), S.end());
    int n = v.size();
    for (int a = 0; a < n; a++) {
        for (int b = a + 1; b < n; b++) {
            Pt p = v[a];
            Pt q = v[b];
            if (p.first == q.first || p.second == q.second) continue;
            int x1 = p.first, x2 = q.first;
            if (x1 > x2) { int aux = x1; x1 = x2; x2 = aux; }
            int t1 = p.second, t2 = q.second;
            if (t1 > t2) { int aux = t1; t1 = t2; t2 = aux; }
            bool hay = false;
            for (int i = 0; i < n; i++) {
                Pt r = v[i];
                if (r != p && r != q && x1 <= r.first && r.first <= x2 && t1 <= r.second && r.second <= t2) {
                    hay = true;
                    break;
                }
            }
            if (!hay) return false;
        }
    }
    return true;
}

set<Pt> greedy(const vector<int>& X) {
    int n = 0;
    for (int i = 0; i < (int)X.size(); i++) if (X[i] > n) n = X[i];
    vector<int> ult(n + 2, 0);  // ultima vez que se toco cada llave
    set<Pt> S;
    for (int i = 1; i <= (int)X.size(); i++) {
        int x = X[i - 1];
        vector<int> fila;
        fila.push_back(x);
        int mejor = ult[x];
        // derecha
        for (int y = x + 1; y <= n; y++) {
            if (ult[y] > mejor) {
                fila.push_back(y);
                mejor = ult[y];
            }
        }
        mejor = ult[x];
        // izquierda
        for (int y = x - 1; y >= 1; y--) {
            if (ult[y] > mejor) {
                fila.push_back(y);
                mejor = ult[y];
            }
        }
        for (int j = 0; j < (int)fila.size(); j++) {
            S.insert(Pt(fila[j], i));
            ult[fila[j]] = i;
        }
    }
    return S;
}

// fuerza bruta, solo para casos chiquitos
set<Pt> optFuerzaBruta(const vector<int>& X) {
    int n = 0;
    for (int i = 0; i < (int)X.size(); i++) if (X[i] > n) n = X[i];
    int m = X.size();
    set<Pt> base;
    for (int i = 1; i <= m; i++) base.insert(Pt(X[i - 1], i));
    vector<Pt> libres;
    for (int t = 1; t <= m; t++)
        for (int k = 1; k <= n; k++)
            if (base.count(Pt(k, t)) == 0) libres.push_back(Pt(k, t));
    set<Pt> mejor;
    int mejorTam = 1 << 30;
    int L = libres.size();
    for (int mask = 0; mask < (1 << L); mask++) {
        int bits = 0;
        for (int j = 0; j < L; j++) if ((mask >> j) & 1) bits++;
        if (bits + (int)base.size() >= mejorTam) continue;
        set<Pt> S = base;
        for (int j = 0; j < L; j++) if ((mask >> j) & 1) S.insert(libres[j]);
        if (satisfecho(S)) {
            mejor = S;
            mejorTam = S.size();
        }
    }
    return mejor;
}

void dibujar(const set<Pt>& S, const vector<int>& X) {
    int n = 0, m = 0;
    for (set<Pt>::iterator it = S.begin(); it != S.end(); it++) {
        if (it->first > n) n = it->first;
        if (it->second > m) m = it->second;
    }
    for (int t = m; t >= 1; t--) {
        cout << "  t=" << t << " |";
        for (int k = 1; k <= n; k++) {
            if (S.count(Pt(k, t)) == 0) cout << " .";
            else if (t <= (int)X.size() && X[t - 1] == k) cout << " X";
            else cout << " o";
        }
        cout << "\n";
    }
    cout << "        ";
    for (int k = 1; k <= n; k++) cout << " " << k;
    cout << "   (X = acceso, o = tocado extra)\n";
}

int main() {
    // ej 10
    set<Pt> E10 = {{1, 1}, {4, 2}};
    set<Pt> E10b = {{1, 1}, {4, 2}, {1, 2}};
    cout << "Ej.10  {(1,1),(4,2)} satisfecho? " << (satisfecho(E10) ? "si" : "no")
         << "  -> agregar (1,2) o (4,1): " << (satisfecho(E10b) ? "si" : "no") << "\n\n";

    // ej 15
    vector<int> X = {2, 1, 3};
    set<Pt> G = greedy(X);
    cout << "Ej.15  Greedy sobre 2,1,3  (costo = " << G.size() << " puntos, satisfecho = "
         << (satisfecho(G) ? "si" : "no") << ")\n";
    dibujar(G, X);
    set<Pt> O = optFuerzaBruta(X);
    cout << "OPT (fuerza bruta) = " << O.size() << " puntos\n";
    dibujar(O, X);
    return 0;
}
