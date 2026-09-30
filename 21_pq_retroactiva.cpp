// cola de prioridad min parcialmente retroactiva
// puente t': Q_t' esta contenido en Q_now
// cada cambio en el pasado cambia Q_now en un solo elemento
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <climits>
#include <cstdlib>
using namespace std;

const double MENOS_INF = -1e18, MAS_INF = 1e18;

struct Op {
    bool esInsert;
    int k;
};

struct PQRetroactiva {
    map<double, Op> ops;
    set<int> Qnow;

    struct Sim {
        map<double, int> borrado;  // tiempo del delete -> que borro
        map<int, double> tInsert;
        vector<double> puentes;
        set<int> final;
        bool valida = true;
    };

    Sim simular() const {
        Sim s;
        set<int> Q;
        map<double, Op>::const_iterator it;
        for (it = ops.begin(); it != ops.end(); it++) {
            double t = it->first;
            Op op = it->second;
            if (op.esInsert) {
                Q.insert(op.k);
                s.tInsert[op.k] = t;
            }
            else if (Q.empty()) {
                s.valida = false;
            }
            else {
                s.borrado[t] = *Q.begin();
                Q.erase(Q.begin());
            }
        }
        s.final = Q;
        // puentes
        s.puentes.push_back(MENOS_INF);
        int fuera = 0;
        set<int> Q2;
        for (it = ops.begin(); it != ops.end(); it++) {
            double t = it->first;
            Op op = it->second;
            if (op.esInsert) {
                Q2.insert(op.k);
                if (s.final.count(op.k) == 0) fuera++;
            } else if (!Q2.empty()) {
                int x = *Q2.begin();
                Q2.erase(Q2.begin());
                if (s.final.count(x) == 0) fuera--;
            }
            if (fuera == 0) s.puentes.push_back(t);
        }
        return s;
    }
    static double ultimoPuenteHasta(const Sim& s, double t, bool estricto) {
        double r = MENOS_INF;
        for (int i = 0; i < (int)s.puentes.size(); i++) {
            double p = s.puentes[i];
            if (estricto) {
                if (p < t) r = p;
            } else {
                if (p <= t) r = p;
            }
        }
        return r;
    }
    static double primerPuenteDesde(const Sim& s, double t) {
        for (int i = 0; i < (int)s.puentes.size(); i++)
            if (s.puentes[i] >= t) return s.puentes[i];
        return MAS_INF;
    }
    static int maxBorradoDespues(const Sim& s, double t, int base) {
        int m = base;
        map<double, int>::const_iterator it;
        for (it = s.borrado.begin(); it != s.borrado.end(); it++) {
            if (it->first > t && it->second > m) m = it->second;
        }
        return m;
    }

    void insertarInsert(double t, int k) {
        Sim s = simular();
        double tp = ultimoPuenteHasta(s, t, false);
        Qnow.insert(maxBorradoDespues(s, tp, k));
        Op op; op.esInsert = true; op.k = k;
        ops[t] = op;
    }
    void insertarDeleteMin(double t) {
        Sim s = simular();
        // a = min de Q_t
        set<int> Qt;
        for (map<double, Op>::iterator it = ops.begin(); it != ops.end(); it++) {
            if (it->first > t) break;
            if (it->second.esInsert) Qt.insert(it->second.k);
            else if (!Qt.empty()) Qt.erase(Qt.begin());
        }
        int a;
        if (Qt.empty()) a = INT_MAX;
        else a = *Qt.begin();
        int sale = INT_MAX;
        if (Qnow.count(a) > 0) {
            sale = a;
        }
        else {
            // cascada
            double da = 0;
            for (map<double, int>::iterator it = s.borrado.begin(); it != s.borrado.end(); it++)
                if (it->second == a) da = it->first;
            double tp = primerPuenteDesde(s, da);
            for (set<int>::iterator it = Qnow.begin(); it != Qnow.end(); it++) {
                if (s.tInsert[*it] <= tp) {
                    sale = *it;
                    break;
                }
            }
        }
        Qnow.erase(sale);
        Op op; op.esInsert = false; op.k = 0;
        ops[t] = op;
    }
    void borrarDeleteMin(double t) {
        Sim s = simular();
        double tp = ultimoPuenteHasta(s, t, true);
        Qnow.insert(maxBorradoDespues(s, tp, INT_MIN));
        ops.erase(t);
    }
    void borrarInsert(double t) {
        int k = ops[t].k;
        if (Qnow.count(k) > 0) {
            Qnow.erase(k);
            ops.erase(t);
            return;
        }
        Sim s = simular();
        double tk = 0;
        for (map<double, int>::iterator it = s.borrado.begin(); it != s.borrado.end(); it++)
            if (it->second == k) tk = it->first;
        ops.erase(t);
        ops.erase(tk);
        insertarDeleteMin(tk);
    }
    int minimoAhora() { return *Qnow.begin(); }
};

void mostrar(PQRetroactiva& P) {
    for (set<int>::iterator it = P.Qnow.begin(); it != P.Qnow.end(); it++) cout << *it << " ";
}

int main() {
    PQRetroactiva P;
    P.insertarInsert(1, 50);
    P.insertarInsert(2, 30);
    P.insertarDeleteMin(3);
    P.insertarInsert(4, 40);
    P.insertarDeleteMin(5);
    cout << "Q_now: "; mostrar(P); cout << "\n";
    P.insertarInsert(0.5, 10);  // retroactivo
    cout << "tras Insert(0.5, insert 10): Q_now: "; mostrar(P); cout << " (entró 40)\n";
    P.insertarDeleteMin(0.7);
    cout << "tras Insert(0.7, delete-min): Q_now: "; mostrar(P); cout << "\n";
    return 0;
}
