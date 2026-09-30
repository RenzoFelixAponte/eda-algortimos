// =====================================================================
// 14 - FRACTIONAL CASCADING  — Semana 5
// ---------------------------------------------------------------------
// Problema: k listas ordenadas L1..Lk (cada una <= n). Para un x, dar en
// CADA lista el primer elemento >= x (su "sucesor").
//   - ingenuo: k búsquedas binarias -> O(k lg n)
//   - fractional cascading:           -> O(k + lg n)
//
// Construcción (de abajo hacia arriba):
//   M_k = L_k
//   M_i = L_i  mezclado con  UNO DE CADA DOS elementos de M_{i+1}
//   => |M_i| <= |L_i| + |M_{i+1}|/2  => espacio total O(total de L)
// Cada elemento de M_i guarda dos punteros:
//   propio[j] = índice en L_i del primer elemento >= M_i[j]
//   baja[j]   = índice en M_{i+1} del primer elemento >= M_i[j]
//
// Consulta: UNA binaria en M_1; luego en cada nivel sigo "baja" y
// retrocedo a lo mucho 1 posición (entre dos promovidos hay a lo mucho
// 1 elemento no promovido). => O(lg n) + O(1) por lista.
// =====================================================================
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
using namespace std;

typedef int Llave;

struct FractionalCascading {
    int k;
    vector<vector<Llave>> L;        // listas originales
    vector<vector<Llave>> M;        // listas aumentadas
    vector<vector<int>> propio, baja;
    long long pasosAtras = 0;       // para comprobar que es <= 1 por nivel

    FractionalCascading(const vector<vector<Llave>>& listas) : k((int)listas.size()), L(listas) {
        M.resize(k); propio.resize(k); baja.resize(k);
        M[k - 1] = L[k - 1];
        for (int i = k - 2; i >= 0; i--) {
            vector<Llave> promovidos;
            for (int j = 1; j < (int)M[i + 1].size(); j += 2) promovidos.push_back(M[i + 1][j]);
            M[i].resize(L[i].size() + promovidos.size());
            merge(L[i].begin(), L[i].end(), promovidos.begin(), promovidos.end(), M[i].begin());
        }
        // punteros (con merge lineal se hace en O(|M_i|); aquí con lower_bound por claridad)
        for (int i = 0; i < k; i++) {
            for (Llave v : M[i]) {
                propio[i].push_back((int)(lower_bound(L[i].begin(), L[i].end(), v) - L[i].begin()));
                if (i + 1 < k)
                    baja[i].push_back((int)(lower_bound(M[i + 1].begin(), M[i + 1].end(), v) - M[i + 1].begin()));
            }
        }
    }

    // resp[i] = índice en L_i del primer elemento >= x (|L_i| si no hay)
    vector<int> buscar(Llave x) {
        vector<int> resp(k);
        int p = (int)(lower_bound(M[0].begin(), M[0].end(), x) - M[0].begin());   // única binaria
        for (int i = 0; i < k; i++) {
            resp[i] = (p == (int)M[i].size()) ? (int)L[i].size() : propio[i][p];
            if (i + 1 == k) break;
            int q = (p == (int)M[i].size()) ? (int)M[i + 1].size() : baja[i][p];
            while (q > 0 && M[i + 1][q - 1] >= x) { q--; pasosAtras++; }            // a lo mucho 1
            p = q;
        }
        return resp;
    }
};

int main() {
    // listas del ejemplo de tu Notion (semana 4)
    vector<vector<Llave>> listas = {
        {2, 5, 8, 12, 15},
        {3, 5, 9, 12, 18},
        {4, 9, 13, 18, 22}};
    FractionalCascading F(listas);
    for (int i = 0; i < F.k; i++) {
        cout << "M" << i + 1 << ": ";
        for (Llave v : F.M[i]) cout << v << " ";
        cout << "\n";
    }
    Llave x = 9;
    vector<int> r = F.buscar(x);
    cout << "sucesor de " << x << ": ";
    for (int i = 0; i < F.k; i++)
        cout << "L" << i + 1 << "->" << (r[i] < (int)listas[i].size() ? to_string(listas[i][r[i]]) : "none") << "  ";
    cout << "\n";

    // prueba aleatoria vs k binarias
    srand(9);
    int K = 20;
    vector<vector<Llave>> Ls(K);
    for (auto& l : Ls) { int n = rand() % 200; for (int j = 0; j < n; j++) l.push_back(rand() % 5000); sort(l.begin(), l.end()); }
    FractionalCascading G(Ls); bool ok = true; int Q = 20000;
    for (int q = 0; q < Q; q++) {
        Llave y = rand() % 5200 - 100;
        vector<int> res = G.buscar(y);
        for (int i = 0; i < K; i++)
            if (res[i] != (int)(lower_bound(Ls[i].begin(), Ls[i].end(), y) - Ls[i].begin())) ok = false;
    }
    cout << "prueba aleatoria: " << (ok ? "OK" : "FALLO")
         << " | pasos atrás por nivel = " << (double)G.pasosAtras / (Q * (K - 1)) << " (<= 1)\n";
    return 0;
}
