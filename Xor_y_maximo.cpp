#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <cstdio>
#include <algorithm>

using namespace std;

const int BITS = 20;

template <typename T>
struct Persistente_parcial {
    vector<array<int, 2>> hijo;
    vector<int> cnt;
    vector<int> ver;
    vector<int> tam;

    void limpiar(int n){
        hijo.clear(); cnt.clear(); ver.clear(); tam.clear();
        hijo.reserve((size_t)(n + 1) * (BITS + 1) + 1);
        cnt.reserve((size_t)(n + 1) * (BITS + 1) + 1);
        hijo.push_back({0, 0});
        cnt.push_back(0);
        ver.push_back(0);
    }

    int vercion_actual(){
        return ver.back();
    }

    int copiar(int nodo){
        hijo.push_back(hijo[nodo]);
        cnt.push_back(cnt[nodo] + 1);
        return hijo.size() - 1;
    }

    void push_new_vercion(T valor_nuevo){
        int anterior = vercion_actual();
        int nuevo = copiar(anterior);
        int cur = nuevo;
        for (int b = BITS - 1; b >= 0; b--){
            int bit = (valor_nuevo >> b) & 1;
            int sig = copiar(hijo[cur][bit]);
            hijo[cur][bit] = sig;
            cur = sig;
        }
        ver.push_back(nuevo);
        tam.push_back(ver.size());
    }
};

long long contar_xor_mayor(int x, int K, Persistente_parcial<int>& p, int l, int r){
    if (l > r) return 0;
    int u = p.ver[r], v = p.ver[l - 1];
    long long res = 0;
    for (int b = BITS - 1; b >= 0; b--){
        int xb = (x >> b) & 1, kb = (K >> b) & 1;
        if (kb == 0){

            res += p.cnt[p.hijo[u][xb ^ 1]] - p.cnt[p.hijo[v][xb ^ 1]];
            u = p.hijo[u][xb]; v = p.hijo[v][xb];
        } else {
            u = p.hijo[u][xb ^ 1]; v = p.hijo[v][xb ^ 1];
        }
        if (p.cnt[u] - p.cnt[v] == 0) break;
    }
    return res;
}

char buffer_entrada[1 << 16];
int buf_len = 0, buf_pos = 0;

int gc(){
    if (buf_pos == buf_len){
        buf_len = fread(buffer_entrada, 1, sizeof(buffer_entrada), stdin);
        buf_pos = 0;
        if (buf_len <= 0) return -1;
    }
    return buffer_entrada[buf_pos++];
}

int leer(){
    int c = gc();
    while (c != -1 && (c < '0' || c > '9')) c = gc();
    int x = 0;
    while (c >= '0' && c <= '9'){ x = x * 10 + (c - '0'); c = gc(); }
    return x;
}

void rangos_maximo(vector<int>& A, int n, vector<int>& L, vector<int>& R){
    vector<int> st;
    L.assign(n + 1, 1); R.assign(n + 1, n);
    for (int i = 1; i <= n; st.push_back(i++)){
        while (!st.empty() && A[st.back()] < A[i]) st.pop_back();
        if (!st.empty()) L[i] = st.back() + 1;
    }
    st.clear();
    for (int i = n; i >= 1; st.push_back(i--)){
        while (!st.empty() && A[st.back()] <= A[i]) st.pop_back();
        if (!st.empty()) R[i] = st.back() - 1;
    }
}

long long suma_con_maximo(int m, vector<int>& A, vector<int>& L, vector<int>& R, Persistente_parcial<int>& p){
    long long pares = 0;
    if (m - L[m] <= R[m] - m)
        for (int i = L[m]; i <= m; i++) pares += contar_xor_mayor(A[i], A[m], p, max(i + 1, m), R[m]);
    else
        for (int j = m; j <= R[m]; j++) pares += contar_xor_mayor(A[j], A[m], p, L[m], min(j - 1, m));
    return pares * A[m];
}

int main (){
    int t = leer();
    Persistente_parcial<int> p;
    vector<int> A, L, R;
    string out;

    while (t--){
        int n = leer();
        A.assign(n + 1, 0);
        p.limpiar(n);
        for (int i = 1; i <= n; i++){
            A[i] = leer();
            p.push_new_vercion(A[i]);
        }

        rangos_maximo(A, n, L, R);
        long long ans = 0;
        for (int m = 1; m <= n; m++) ans += suma_con_maximo(m, A, L, R, p);
        out += to_string(ans) + '\n';
    }
    cout << out;
}
