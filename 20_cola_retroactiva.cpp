// =====================================================================
// 20 - COLA (FIFO) TOTALMENTE RETROACTIVA  — Retroactividad (Ej. 3 y 8)
// ---------------------------------------------------------------------
// Retroactividad = modificar el PASADO de la línea de tiempo:
//   Insert(t, op)  : meter una operación en el tiempo t
//   Delete(t)      : borrar la operación del tiempo t
//   Query(t, ...)  : preguntar el estado en el tiempo t
// PARCIAL: Query solo en el presente.  TOTAL: Query en cualquier t.
// (Persistencia = ramas nuevas de versiones; retroactividad = cambiar
//  la historia y que el cambio se PROPAGUE hasta el presente.)
//
// Cola: el frente en el tiempo t es el (d+1)-ésimo enqueue en orden de
// tiempo, donde d = # dequeues con tiempo <= t. Basta con 2 árboles con
// estadística de orden (uno de enqueues, uno de dequeues) => O(lg m)
// por operación y por consulta, TOTALMENTE retroactiva.
// Supuesto: la historia es VÁLIDA (nunca se hace dequeue con la cola vacía).
//
// (Operaciones CONMUTATIVAS e INVERTIBLES, Ej. 8: si las operaciones
//  conmutan (a+b = b+a) e invierten, Insert(t, op) = aplicar op ahora y
//  Delete(t, op) = aplicar op^{-1} ahora -> retroactividad parcial gratis.
//  La cola NO es conmutativa, por eso necesita esta estructura.)
// =====================================================================
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

// ---------- fuerza bruta para verificar ----------
bool historiaValida(const map<Tiempo, pair<int, int>>& ops) {
    int tam = 0;
    for (auto& [tt, op] : ops) {
        if (op.first == 0) tam++;
        else if (tam-- == 0) return false;
    }
    return true;
}
int frenteBruto(const map<Tiempo, pair<int, int>>& ops, Tiempo t) {   // (tipo 0=enq 1=deq, valor)
    vector<int> q; size_t cab = 0;
    for (auto& [tt, op] : ops) {
        if (tt > t) break;
        if (op.first == 0) q.push_back(op.second);
        else cab++;
    }
    return cab < q.size() ? q[cab] : -1;
}

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

    // prueba aleatoria vs simulación
    srand(12);
    ColaRetroactiva R; map<Tiempo, pair<int, int>> ops; bool ok = true; int comparadas = 0;
    for (int it = 0; it < 3000; it++) {
        Tiempo t = rand() % 100000 + (rand() % 1000) / 1000.0;
        if (ops.count(t)) continue;
        int tipo = rand() % 5 < 3 ? 0 : rand() % 2 + 1;   // más enqueues para que la historia sea válida
        if (tipo == 0) { int x = rand() % 1000; R.insertarEnqueue(t, x); ops[t] = {0, x}; }
        else if (tipo == 1) { R.insertarDequeue(t); ops[t] = {1, 0}; }
        else if (!ops.empty()) {                                   // borrar una op existente
            auto itv = ops.lower_bound(t); if (itv == ops.end()) itv = ops.begin();
            if (itv->second.first == 0) R.borrarEnqueue(itv->first); else R.borrarDequeue(itv->first);
            ops.erase(itv);
        }
        Tiempo q = rand() % 100000;
        if (!historiaValida(ops)) continue;
        comparadas++;
        if (R.frente(q) != frenteBruto(ops, q)) ok = false;
    }
    cout << "prueba aleatoria (" << comparadas << " consultas con historia válida): " << (ok ? "OK" : "FALLO") << "\n";
    return 0;
}
