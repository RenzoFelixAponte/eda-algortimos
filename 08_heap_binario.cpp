// heap binario min, arreglo desde 1
// buildHeap es O(n), insertando uno por uno es n lg n
// [3,2,1] da distinto con cada metodo
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

long long swaps = 0;

struct HeapBinario {
    vector<int> A;  // A[0] no se usa
    HeapBinario() { A.push_back(0); }

    int n() { return (int)A.size() - 1; }
    bool vacio() {
        if (n() == 0) return true;
        return false;
    }
    int minimo() { return A[1]; }

    void subir(int i) {
        while (i > 1 && A[i] < A[i / 2]) {
            int aux = A[i];
            A[i] = A[i / 2];
            A[i / 2] = aux;
            swaps++;
            i = i / 2;
        }
    }
    void heapify(int i) {
        while (true) {
            int m = i;
            int l = 2 * i;
            int r = 2 * i + 1;
            if (l <= n() && A[l] < A[m]) m = l;
            if (r <= n() && A[r] < A[m]) m = r;
            if (m == i) return;
            int aux = A[i];
            A[i] = A[m];
            A[m] = aux;
            swaps++;
            i = m;
        }
    }
    void insertar(int x) {
        A.push_back(x);
        subir(n());
    }
    int extraerMin() {
        int r = A[1];
        A[1] = A.back();
        A.pop_back();
        if (!vacio()) heapify(1);
        return r;
    }
    void decreaseKey(int i, int x) {
        A[i] = x;
        subir(i);
    }

    void buildHeap(const vector<int>& v) {
        A.clear();
        A.push_back(0);
        for (int i = 0; i < (int)v.size(); i++) A.push_back(v[i]);
        for (int i = n() / 2; i >= 1; i--) heapify(i);
    }
    void buildHeapInsert(const vector<int>& v) {
        A.clear();
        A.push_back(0);
        for (int i = 0; i < (int)v.size(); i++) insertar(v[i]);
    }
    void imprimir(const char* nom) {
        cout << nom << ": [";
        for (int i = 1; i <= n(); i++) {
            cout << A[i];
            if (i < n()) cout << ",";
        }
        cout << "]\n";
    }
};

int main() {
    HeapBinario H1, H2;
    H1.buildHeap({3, 2, 1});
    H1.imprimir("Build-Heap        [3,2,1]");
    H2.buildHeapInsert({3, 2, 1});
    H2.imprimir("Build-Heap-Insert [3,2,1]");

    // entrada decreciente = peor caso
    int tams[3] = {1 << 10, 1 << 14, 1 << 18};
    for (int k = 0; k < 3; k++) {
        int n = tams[k];
        vector<int> dec(n);
        for (int i = 0; i < n; i++) dec[i] = n - i;
        HeapBinario a, b;
        swaps = 0;
        a.buildHeap(dec);
        long long s1 = swaps;
        swaps = 0;
        b.buildHeapInsert(dec);
        long long s2 = swaps;
        cout << "n=" << n << "  buildHeap swaps/n=" << (double)s1 / n
             << "   buildHeapInsert swaps/n=" << (double)s2 / n << " (crece como lg n)\n";
    }

    // heapsort
    HeapBinario H;
    H.buildHeap({9, 4, 7, 1, 8, 2});
    cout << "ordenado: ";
    while (!H.vacio()) cout << H.extraerMin() << " ";
    cout << "\n";
    return 0;
}
