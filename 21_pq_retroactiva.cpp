// =====================================================================
// 21 - COLA DE PRIORIDAD PARCIALMENTE RETROACTIVA (min)  — Ej. 13 (puentes)
// ---------------------------------------------------------------------
// Línea de tiempo de operaciones insert(k) y delete-min. Q_t = estado
// en el tiempo t; Q_now = estado actual. Parcial: solo se consulta Q_now.
//
// PUENTE: un tiempo t' es puente si Q_{t'} ⊆ Q_now (todo lo que estaba
// en t' sobrevive hasta hoy). -INF y "ahora" siempre son puentes.
//
// Reglas (Demaine, Iacono, Langerman) — cada cambio en el pasado
// modifica Q_now en EXACTAMENTE UN elemento:
//   Insert(t, insert(k)):  t' = último puente <= t
//        entra a Q_now  max( k, max{ k' borrado después de t' } )
//   Insert(t, delete-min): sea a = min(Q_t) (lo que borra el nuevo delete).
//        Efecto en Q_now == haber quitado el insert(a). Entonces:
//        si a in Q_now -> sale a
//        si no (a se borraba en d_a): t' = primer puente >= d_a,
//        sale  min{ k in Q_now insertado antes de t' }
//   Delete(t, delete-min) (que había borrado k): t' = último puente < t
//        entra a Q_now  max{ k' borrado después de t' }   (incluye a k)
//   Delete(t, insert(k)):  si k in Q_now -> sale k
//        si no (k fue borrado en t_k): quitar ambas ops y aplicar
//        Insert(t_k, delete-min).
//
// Aquí puentes y máximos se calculan simulando la línea O(m lg m) para
// que se vea la idea; la versión eficiente guarda la línea de tiempo en
// un BST balanceado aumentado (prefijos de +1/-1 y max/min por subárbol)
// y logra O(lg m) por operación retroactiva.
// Supuesto: llaves distintas e historia válida (no delete-min en vacío).
// =====================================================================
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

    // prueba aleatoria: historia válida al azar + UN cambio retroactivo, vs simular desde cero
    srand(77);
    const char* nombre[4] = {"Insert(t, insert)", "Insert(t, delete-min)", "Delete(delete-min)", "Delete(insert)"};
    int fallos[4] = {0}, total[4] = {0};
    for (int prueba = 0; prueba < 20000; prueba++) {
        PQRetroactiva R; int llave = 1;
        int m = rand() % 10 + 1;
        for (int i = 1; i <= m; i++) {
            if (rand() % 3) R.ops[i] = {true, (llave++ * 37) % 1009};
            else R.ops[i] = {false, 0};
        }
        PQRetroactiva::Sim s = R.simular();
        if (!s.valida) continue;
        R.Qnow = s.final;
        int tipo = rand() % 4;
        Tiempo t = rand() % (m + 1) + 0.5;
        auto it = R.ops.begin(); advance(it, rand() % R.ops.size());
        if (tipo == 0) R.insertarInsert(t, (llave * 37) % 1009);
        else if (tipo == 1) R.insertarDeleteMin(t);
        else if (tipo == 2) { if (it->second.esInsert) continue; R.borrarDeleteMin(it->first); }
        else { if (!it->second.esInsert) continue; R.borrarInsert(it->first); }
        PQRetroactiva::Sim s2 = R.simular();
        if (!s2.valida) continue;
        total[tipo]++;
        if (s2.final != R.Qnow) fallos[tipo]++;
    }
    for (int i = 0; i < 4; i++)
        cout << "  " << nombre[i] << ": " << (fallos[i] ? "FALLO " : "OK ") << total[i] - fallos[i] << "/" << total[i] << "\n";
    return 0;
}
