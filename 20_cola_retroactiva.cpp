// cola totalmente retroactiva
// el frente en t es el (d+1)-esimo enqueue, d = dequeues hasta t
#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
using namespace std;
using namespace __gnu_pbds;

// arbol con order_of_key y find_by_order
typedef tree<double, null_type, less<double>, rb_tree_tag, tree_order_statistics_node_update> ArbolOrden;

struct ColaRetroactiva {
    ArbolOrden enq, deq;
    map<double, int> valor;

    void insertarEnqueue(double t, int x) {
        enq.insert(t);
        valor[t] = x;
    }
    void insertarDequeue(double t) { deq.insert(t); }
    void borrarEnqueue(double t) {
        enq.erase(t);
        valor.erase(t);
    }
    void borrarDequeue(double t) { deq.erase(t); }

    int frente(double t) {
        double t2 = nextafter(t, 1e18);  // para contar los <= t
        int d = deq.order_of_key(t2);
        int e = enq.order_of_key(t2);
        if (d >= e) return -1;  // vacia
        double aux = *enq.find_by_order(d);
        return valor[aux];
    }
    int tamano(double t) {
        double t2 = nextafter(t, 1e18);
        int d = deq.order_of_key(t2);
        int e = enq.order_of_key(t2);
        if (e - d < 0) return 0;
        return e - d;
    }
};

int main() {
    ColaRetroactiva C;
    C.insertarEnqueue(1, 10);
    C.insertarEnqueue(2, 20);
    C.insertarDequeue(3);
    C.insertarEnqueue(4, 30);
    cout << "frente en t=2: " << C.frente(2) << "  t=3: " << C.frente(3) << "  ahora: " << C.frente(100) << "\n";
    C.insertarEnqueue(0.5, 5);  // en el pasado
    cout << "tras Insert(0.5, enqueue 5): frente en t=3: " << C.frente(3) << "  ahora: " << C.frente(100) << "\n";
    C.borrarDequeue(3);
    cout << "tras Delete(3): frente ahora: " << C.frente(100) << ", tamaño ahora: " << C.tamano(100) << "\n";
    return 0;
}
