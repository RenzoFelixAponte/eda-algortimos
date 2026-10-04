#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <algorithm>

using namespace std;

const int BITS = 30; 

template <typename T>
struct Persistente_parcial {
    vector<array<int, 2>> hijo; 
    vector<int> ver;           
    vector<int> tam;

    Persistente_parcial(){
        hijo.push_back({0, 0});
    }

    int vercion_actual(){
        if (ver.empty()) return 0;
        return ver.back();            
    }

    void push_new_vercion(T valor_nuevo){
        int anterior = vercion_actual();
        hijo.push_back(hijo[anterior]);         
        int nuevo = hijo.size() - 1;
        int cur = nuevo;
        for (int b = BITS - 1; b >= 0; b--){
            int bit = (valor_nuevo >> b) & 1;
            hijo.push_back(hijo[hijo[cur][bit]]); 
            hijo[cur][bit] = hijo.size() - 1;
            cur = hijo[cur][bit];
        }
        ver.push_back(nuevo);
        tam.push_back(ver.size());
    }
};

int mejor_xor(int x, Persistente_parcial<int>& p, int k){
    int cur = p.ver[k - 1];             
    int mejor = 0;
    for (int b = BITS - 1; b >= 0; b--){
        int quiero = ((x >> b) & 1) ^ 1;   // bit contrario maximiza el xor
        if (p.hijo[cur][quiero]){
            mejor |= (1 << b);
            cur = p.hijo[cur][quiero];
        } else {
            cur = p.hijo[cur][quiero ^ 1];
        }
    }
    return mejor;
}

int main (){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    int q, k, x, a;
    Persistente_parcial<int> p;

    cin >> n;
    p.hijo.reserve((size_t)n * (BITS + 1) + 1);
    for (int i = 0; i < n; i++){
        cin >> a;
        p.push_new_vercion(a);
    }

    cin >> q;
    for (int i = 0; i < q; i++){
        cin >> k >> x;
        cout << mejor_xor(x, p, k) << '\n';
    }
}
