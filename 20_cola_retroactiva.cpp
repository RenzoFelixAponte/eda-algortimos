// cola totalmente retroactiva
// frente en el tiempo t = el (d+1)-esimo enqueue, d = # dequeues <= t
// 2 arboles con estadistica de orden -> O(lg m)
#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
using namespace std;
using namespace __gnu_pbds;

typedef double Tiempo;
// árbol balanceado con order_of_key / find_by_order (estadística de orden)
typedef tree<Tiempo, null_type, less<Tiempo>, rb_tree_tag, tree_order_statistics_node_update> ArbolOrden;

struct ColaRetroactiva {
    ArbolOrden enq, deq;
    map<Tiempo, int> valor;          // tiempo de un enqueue -> valor

    void insertarEnqueue(Tiempo t, int x) { enq.insert(t); valor[t] = x; }
    void insertarDequeue(Tiempo t) { deq.insert(t); }
    void borrarEnqueue(Tiempo t) { enq.erase(t); valor.erase(t); }
    void borrarDequeue(Tiempo t) { deq.erase(t); }

    // frente en el tiempo t (después de aplicar las ops con tiempo <= t); -1 si vacía
    int frente(Tiempo t) {
        int d = (int)deq.order_of_key(nextafter(t, 1e18));   // # dequeues <= t
        int e = (int)enq.order_of_key(nextafter(t, 1e18));   // # enqueues <= t
        if (d >= e) return -1;
        return valor[*enq.find_by_order(d)];
    }
    int tamano(Tiempo t) {
        int d = (int)deq.order_of_key(nextafter(t, 1e18));
        int e = (int)enq.order_of_key(nextafter(t, 1e18));
        return max(0, e - d);
    }
};

int main() {
    ColaRetroactiva C;
    C.insertarEnqueue(1, 10);
    C.insertarEnqueue(2, 20);
    C.insertarDequeue(3);
    C.insertarEnqueue(4, 30);
    cout << "frente en t=2: " << C.frente(2) << "  t=3: " << C.frente(3) << "  ahora: " << C.frente(100) << "\n";
    C.insertarEnqueue(0.5, 5);          // RETROACTIVO: enqueue(5) antes de todo
    cout << "tras Insert(0.5, enqueue 5): frente en t=3: " << C.frente(3) << "  ahora: " << C.frente(100) << "\n";
    C.borrarDequeue(3);                 // RETROACTIVO: nunca hubo dequeue en t=3
    cout << "tras Delete(3): frente ahora: " << C.frente(100) << ", tamaño ahora: " << C.tamano(100) << "\n";
    return 0;
}
