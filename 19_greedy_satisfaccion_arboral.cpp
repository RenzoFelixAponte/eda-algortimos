// vista geometrica de los BST: punto (llave, tiempo)
// arboralmente satisfecho: todo rectangulo entre 2 puntos (no misma fila/columna) tiene otro punto
// greedy: en cada fila toca x_i y la escalera de llaves que arregla los rectangulos vacios
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cstdlib>
using namespace std;

typedef pair<int, int> Pt;      // (llave, tiempo)

bool satisfecho(const set<Pt>& S) {
    vector<Pt> v(S.begin(), S.end());
    for (size_t a = 0; a < v.size(); a++)
        for (size_t b = a + 1; b < v.size(); b++) {
            Pt p = v[a], q = v[b];
            if (p.first == q.first || p.second == q.second) continue;
            int x1 = min(p.first, q.first), x2 = max(p.first, q.first);
            int t1 = min(p.second, q.second), t2 = max(p.second, q.second);
            bool hay = false;
            for (const Pt& r : v)
                if (r != p && r != q && x1 <= r.first && r.first <= x2 && t1 <= r.second && r.second <= t2) { hay = true; break; }
            if (!hay) return false;
        }
    return true;
}

// X = secuencia de accesos (llaves 1..n), tiempos 1..m
set<Pt> greedy(const vector<int>& X) {
    int n = *max_element(X.begin(), X.end());
    vector<int> ult(n + 2, 0);                 // última fila en que se tocó cada llave (0 = nunca)
    set<Pt> S;
    for (int i = 1; i <= (int)X.size(); i++) {
        int x = X[i - 1];
        vector<int> fila = {x};
        int mejor = ult[x];
        for (int y = x + 1; y <= n; y++)       // escalera a la derecha
            if (ult[y] > mejor) { fila.push_back(y); mejor = ult[y]; }
        mejor = ult[x];
        for (int y = x - 1; y >= 1; y--)       // escalera a la izquierda
            if (ult[y] > mejor) { fila.push_back(y); mejor = ult[y]; }
        for (int y : fila) { S.insert({y, i}); ult[y] = i; }
    }
    return S;
}

// OPT por fuerza bruta (solo para casos MUY chicos): menor superconjunto satisfecho
set<Pt> optFuerzaBruta(const vector<int>& X) {
    int n = *max_element(X.begin(), X.end()), m = (int)X.size();
    set<Pt> base;
    for (int i = 1; i <= m; i++) base.insert({X[i - 1], i});
    vector<Pt> libres;
    for (int t = 1; t <= m; t++) for (int k = 1; k <= n; k++) if (!base.count({k, t})) libres.push_back({k, t});
    set<Pt> mejor; int mejorTam = 1 << 30;
    for (int mask = 0; mask < (1 << libres.size()); mask++) {
        if (__builtin_popcount(mask) + (int)base.size() >= mejorTam) continue;
        set<Pt> S = base;
        for (size_t j = 0; j < libres.size(); j++) if (mask >> j & 1) S.insert(libres[j]);
        if (satisfecho(S)) { mejor = S; mejorTam = (int)S.size(); }
    }
    return mejor;
}

void dibujar(const set<Pt>& S, const vector<int>& X) {
    int n = 0, m = 0;
    for (auto& p : S) { n = max(n, p.first); m = max(m, p.second); }
    for (int t = m; t >= 1; t--) {                 // tiempo hacia arriba
        cout << "  t=" << t << " |";
        for (int k = 1; k <= n; k++) {
            if (!S.count({k, t})) cout << " .";
            else cout << (t <= (int)X.size() && X[t - 1] == k ? " X" : " o");
        }
        cout << "\n";
    }
    cout << "        ";
    for (int k = 1; k <= n; k++) cout << " " << k;
    cout << "   (X = acceso, o = tocado extra)\n";
}

int main() {
    // Ejercicio 10: {(1,1), (4,2)}
    set<Pt> E10 = {{1, 1}, {4, 2}};
    cout << "Ej.10  {(1,1),(4,2)} satisfecho? " << (satisfecho(E10) ? "si" : "no")
         << "  -> agregar (1,2) o (4,1): " << (satisfecho({{1, 1}, {4, 2}, {1, 2}}) ? "si" : "no") << "\n\n";

    // Ejercicio 15: Greedy con la secuencia 2, 1, 3
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
