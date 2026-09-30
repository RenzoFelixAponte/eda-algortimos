// heap binario (min) en arreglo 1-indexado
// buildHeap O(n) vs buildHeapInsert Theta(n lg n)
// contraejemplo min-heap: [3,2,1] -> [1,2,3] vs [1,3,2]  (max-heap: [1,2,3])
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

typedef int Llave;
long long swaps = 0;

struct HeapBinario {
    vector<Llave> A;                  // A[0] no se usa
    HeapBinario() { A.push_back(0); }

    int n() { return (int)A.size() - 1; }
    bool vacio() { return n() == 0; }
    Llave minimo() { return A[1]; }

    void subir(int i) {
        while (i > 1 && A[i] < A[i / 2]) {
            swap(A[i], A[i / 2]); swaps++;
            i /= 2;
        }
    }
    void heapify(int i) {             // bajar
        while (true) {
            int m = i, l = 2 * i, r = 2 * i + 1;
            if (l <= n() && A[l] < A[m]) m = l;
            if (r <= n() && A[r] < A[m]) m = r;
            if (m == i) return;
            swap(A[i], A[m]); swaps++;
            i = m;
        }
    }
    void insertar(Llave x) { A.push_back(x); subir(n()); }
    Llave extraerMin() {
        Llave r = A[1];
        A[1] = A.back(); A.pop_back();
        if (!vacio()) heapify(1);
        return r;
    }
    void decreaseKey(int i, Llave nueva) { A[i] = nueva; subir(i); }

    void buildHeap(const vector<Llave>& v) {
        A.assign(1, 0); A.insert(A.end(), v.begin(), v.end());
        for (int i = n() / 2; i >= 1; i--) heapify(i);
    }
    void buildHeapInsert(const vector<Llave>& v) {
        A.assign(1, 0);
        for (Llave x : v) insertar(x);
    }
    void imprimir(const char* nom) {
        cout << nom << ": [";
        for (int i = 1; i <= n(); i++) cout << A[i] << (i < n() ? "," : "");
        cout << "]\n";
    }
};

int main() {
    HeapBinario H1, H2;
    H1.buildHeap({3, 2, 1});        H1.imprimir("Build-Heap        [3,2,1]");
    H2.buildHeapInsert({3, 2, 1});  H2.imprimir("Build-Heap-Insert [3,2,1]");

    // Θ(n lg n) vs O(n): entrada decreciente (peor caso para insertar)
    for (int n : {1 << 10, 1 << 14, 1 << 18}) {
        vector<Llave> dec(n);
        for (int i = 0; i < n; i++) dec[i] = n - i;
        HeapBinario a, b;
        swaps = 0; a.buildHeap(dec);       long long s1 = swaps;
        swaps = 0; b.buildHeapInsert(dec); long long s2 = swaps;
        cout << "n=" << n << "  buildHeap swaps/n=" << (double)s1 / n
             << "   buildHeapInsert swaps/n=" << (double)s2 / n << " (crece como lg n)\n";
    }

    // heapsort con extraerMin
    HeapBinario H;
    H.buildHeap({9, 4, 7, 1, 8, 2});
    cout << "ordenado: ";
    while (!H.vacio()) cout << H.extraerMin() << " ";
    cout << "\n";
    return 0;
}
