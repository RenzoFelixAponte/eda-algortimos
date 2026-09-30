// fractional cascading: buscar x en k listas ordenadas en O(k + lg n)
// M_i = L_i + uno de cada dos elementos de M_{i+1}, con punteros hacia abajo
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
    // listas del ejemplo de clase
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
    return 0;
}
