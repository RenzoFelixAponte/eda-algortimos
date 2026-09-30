// cola de prioridad (min) parcialmente retroactiva
// puente t': Q_t' esta contenido en Q_now
// cada cambio en el pasado cambia Q_now en exactamente un elemento
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <climits>
#include <cstdlib>
using namespace std;

typedef double Tiempo;
const Tiempo MENOS_INF = -1e18, MAS_INF = 1e18;

struct Op { bool esInsert; int k; };

struct PQRetroactiva {
    map<Tiempo, Op> ops;
    set<int> Qnow;

    // ---------- simulación: estado, qué borró cada delete-min y puentes ----------
    struct Sim {
        map<Tiempo, int> borrado;        // tiempo de delete-min -> llave que borró
        map<int, Tiempo> tInsert;        // llave -> tiempo en que se insertó
        vector<Tiempo> puentes;          // ordenados, incluye -INF
        set<int> final;
        bool valida = true;
    };
    Sim simular() const {
        Sim s;
        set<int> Q;
        for (auto& [t, op] : ops) {
            if (op.esInsert) { Q.insert(op.k); s.tInsert[op.k] = t; }
            else if (Q.empty()) s.valida = false;
            else { s.borrado[t] = *Q.begin(); Q.erase(Q.begin()); }
        }
        s.final = Q;
        // puentes: segunda pasada, Q_t ⊆ Q_final <=> # elementos de Q_t fuera de final == 0
        s.puentes.push_back(MENOS_INF);
        int fuera = 0; set<int> Q2;
        for (auto& [t, op] : ops) {
            if (op.esInsert) { Q2.insert(op.k); if (!s.final.count(op.k)) fuera++; }
            else if (!Q2.empty()) { int x = *Q2.begin(); Q2.erase(Q2.begin()); if (!s.final.count(x)) fuera--; }
            if (fuera == 0) s.puentes.push_back(t);
        }
        return s;
    }
    static Tiempo ultimoPuenteHasta(const Sim& s, Tiempo t, bool estricto) {
        Tiempo r = MENOS_INF;
        for (Tiempo p : s.puentes) if (estricto ? p < t : p <= t) r = p;
        return r;
    }
    static Tiempo primerPuenteDesde(const Sim& s, Tiempo t) {
        for (Tiempo p : s.puentes) if (p >= t) return p;
        return MAS_INF;
    }
    static int maxBorradoDespues(const Sim& s, Tiempo t, int base) {
        int m = base;
        for (auto& [td, k] : s.borrado) if (td > t) m = max(m, k);
        return m;
    }

    // ---------- operaciones retroactivas ----------
    void insertarInsert(Tiempo t, int k) {
        Sim s = simular();
        Tiempo tp = ultimoPuenteHasta(s, t, false);
        Qnow.insert(maxBorradoDespues(s, tp, k));
        ops[t] = {true, k};
    }
    void insertarDeleteMin(Tiempo t) {
        Sim s = simular();
        // a = min(Q_t): lo que borraría el nuevo delete-min en su momento
        set<int> Qt;
        for (auto& [tt, op] : ops) {
            if (tt > t) break;
            if (op.esInsert) Qt.insert(op.k); else if (!Qt.empty()) Qt.erase(Qt.begin());
        }
        int a = Qt.empty() ? INT_MAX : *Qt.begin();
        int sale = INT_MAX;
        if (Qnow.count(a)) sale = a;                        // a sobrevivía hasta hoy: sale a
        else {
            // a iba a ser borrado en d_a; ese delete-min ahora borra otra cosa (cascada)
            Tiempo da = 0;
            for (auto& [td, kk] : s.borrado) if (kk == a) da = td;
            Tiempo tp = primerPuenteDesde(s, da);
            for (int k : Qnow) if (s.tInsert[k] <= tp) { sale = k; break; }   // Qnow ordenado: primero = min
        }
        Qnow.erase(sale);
        ops[t] = {false, 0};
    }
    void borrarDeleteMin(Tiempo t) {
        Sim s = simular();
        Tiempo tp = ultimoPuenteHasta(s, t, true);
        Qnow.insert(maxBorradoDespues(s, tp, INT_MIN));
        ops.erase(t);
    }
    void borrarInsert(Tiempo t) {
        int k = ops[t].k;
        if (Qnow.count(k)) { Qnow.erase(k); ops.erase(t); return; }
        Sim s = simular();
        Tiempo tk = 0;
        for (auto& [td, kk] : s.borrado) if (kk == k) tk = td;
        ops.erase(t); ops.erase(tk);          // quitar el par no cambia Q_now
        insertarDeleteMin(tk);                // y el delete-min vuelve a entrar en t_k
    }
    int minimoAhora() { return *Qnow.begin(); }
};

int main() {
    PQRetroactiva P;
    P.insertarInsert(1, 50);
    P.insertarInsert(2, 30);
    P.insertarDeleteMin(3);        // borra 30
    P.insertarInsert(4, 40);
    P.insertarDeleteMin(5);        // borra 40
    cout << "Q_now: "; for (int k : P.Qnow) cout << k << " "; cout << "\n";
    P.insertarInsert(0.5, 10);     // RETROACTIVO: el delete de t=3 ahora borra 10 -> cascada
    cout << "tras Insert(0.5, insert 10): Q_now: "; for (int k : P.Qnow) cout << k << " "; cout << " (entró 40)\n";
    P.insertarDeleteMin(0.7);      // RETROACTIVO
    cout << "tras Insert(0.7, delete-min): Q_now: "; for (int k : P.Qnow) cout << k << " "; cout << "\n";
    return 0;
}
