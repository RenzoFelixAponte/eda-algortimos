// fractional cascading
// buscar x en k listas en O(k + lg n)
// M_i = L_i + la mitad de M_{i+1}
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
using namespace std;

struct FractionalCascading {
    int k;
    vector<vector<int>> L;
    vector<vector<int>> M;
    vector<vector<int>> propio, baja;
    long long pasosAtras = 0;

    FractionalCascading(const vector<vector<int>>& listas) {
        k = listas.size();
        L = listas;
        M.resize(k);
        propio.resize(k);
        baja.resize(k);
        M[k - 1] = L[k - 1];
        for (int i = k - 2; i >= 0; i--) {
            vector<int> aux;
            for (int j = 1; j < (int)M[i + 1].size(); j = j + 2) aux.push_back(M[i + 1][j]);
            M[i].resize(L[i].size() + aux.size());
            merge(L[i].begin(), L[i].end(), aux.begin(), aux.end(), M[i].begin());
        }
        // punteros (con lower_bound, mas facil)
        for (int i = 0; i < k; i++) {
            for (int j = 0; j < (int)M[i].size(); j++) {
                int v = M[i][j];
                int n1 = lower_bound(L[i].begin(), L[i].end(), v) - L[i].begin();
                propio[i].push_back(n1);
                if (i + 1 < k) {
                    int n2 = lower_bound(M[i + 1].begin(), M[i + 1].end(), v) - M[i + 1].begin();
                    baja[i].push_back(n2);
                }
            }
        }
    }

    // res[i] = indice del primer >= x en L_i
    vector<int> buscar(int x) {
        vector<int> res(k);
        int p = lower_bound(M[0].begin(), M[0].end(), x) - M[0].begin();  // solo una binaria
        for (int i = 0; i < k; i++) {
            if (p == (int)M[i].size()) res[i] = L[i].size();
            else res[i] = propio[i][p];
            if (i + 1 == k) break;
            int q;
            if (p == (int)M[i].size()) q = M[i + 1].size();
            else q = baja[i][p];
            while (q > 0 && M[i + 1][q - 1] >= x) {
                q--;
                pasosAtras++;
            }
            p = q;
        }
        return res;
    }
};

int main() {
    vector<vector<int>> listas = {
        {2, 5, 8, 12, 15},
        {3, 5, 9, 12, 18},
        {4, 9, 13, 18, 22}};
    FractionalCascading F(listas);
    for (int i = 0; i < F.k; i++) {
        cout << "M" << i + 1 << ": ";
        for (int j = 0; j < (int)F.M[i].size(); j++) cout << F.M[i][j] << " ";
        cout << "\n";
    }
    int x = 9;
    vector<int> r = F.buscar(x);
    cout << "sucesor de " << x << ": ";
    for (int i = 0; i < F.k; i++) {
        cout << "L" << i + 1 << "->";
        if (r[i] < (int)listas[i].size()) cout << listas[i][r[i]];
        else cout << "none";
        cout << "  ";
    }
    cout << "\n";
    return 0;
}
