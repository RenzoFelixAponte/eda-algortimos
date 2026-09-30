// stack persistente (funcional)
// cada version es un puntero al tope, nunca se modifica un nodo -> persistencia total
// push/pop O(1), concat O(|S1|) (copia S1) -> confluente
#include <iostream>
#include <vector>
using namespace std;

template <typename T>
struct NodoS {
    T valor;
    NodoS* sig;
};

template <typename T>
struct StackPersistente {
    vector<NodoS<T>*> ver;   // ver[i] = tope de la versión i
    vector<int> tam;         // tam[i] = cantidad de elementos de la versión i

    StackPersistente() {
        ver.push_back(nullptr);  // versión 0 = stack vacío
        tam.push_back(0);
    }

    int nuevaVersion(NodoS<T>* tope, int t) {
        ver.push_back(tope);
        tam.push_back(t);
        return (int)ver.size() - 1;
    }

    // Push sobre la versión v -> devuelve el número de la nueva versión
    int push(int v, T x) {
        NodoS<T>* nuevo = new NodoS<T>{x, ver[v]};   // v queda intacta
        return nuevaVersion(nuevo, tam[v] + 1);
    }

    // Pop sobre la versión v (si está vacía, la nueva versión también)
    int pop(int v) {
        if (ver[v] == nullptr) return nuevaVersion(nullptr, 0);
        return nuevaVersion(ver[v]->sig, tam[v] - 1);
    }

    bool vacio(int v) { return ver[v] == nullptr; }
    T top(int v) { return ver[v]->valor; }       // asume no vacío

    // Concat: tope de a ... fondo de a -> tope de b ... fondo de b
    // Copia los nodos de a (O(|a|)); los de b se comparten.
    int concat(int a, int b) {
        vector<T> valores;
        for (NodoS<T>* p = ver[a]; p != nullptr; p = p->sig) valores.push_back(p->valor);
        NodoS<T>* cabeza = ver[b];
        for (int i = (int)valores.size() - 1; i >= 0; i--) {
            cabeza = new NodoS<T>{valores[i], cabeza};
        }
        return nuevaVersion(cabeza, tam[a] + tam[b]);
    }

    void imprimir(int v) {
        cout << "v" << v << ": [";
        for (NodoS<T>* p = ver[v]; p != nullptr; p = p->sig) {
            cout << p->valor;
            if (p->sig) cout << " -> ";
        }
        cout << "]  (tam " << tam[v] << ")\n";
    }
};

int main() {
    StackPersistente<int> S;

    // ejemplo extra 2B
    int v1 = S.push(0, 5);    // [5]
    int v2 = S.push(v1, 7);   // [7 -> 5]
    int v3 = S.push(v1, 9);   // [9 -> 5]  <- push sobre v1, que NO es la última => TOTAL
    int v4 = S.pop(v2);       // [5]
    int v5 = S.concat(v2, v3);// [7 -> 5 -> 9 -> 5]  <- combina 2 versiones => CONFLUENTE

    for (int v : {0, v1, v2, v3, v4, v5}) S.imprimir(v);

    // El nodo [5] es compartido por v1, v2 y v3:
    cout << "\nnodo 5 compartido entre v1, v2, v3? "
         << (S.ver[v1] == S.ver[v2]->sig && S.ver[v1] == S.ver[v3]->sig ? "si" : "no") << "\n";
    return 0;
}
