// =====================================================================
// 03 - BST con PERSISTENCIA PARCIAL usando NODOS GORDOS (DSST 1989)
// ---------------------------------------------------------------------
// (Ejercicio de las diapositivas: "Implementa un BST con persistencia
//  parcial usando nodos gordos".)
//
// Cada nodo guarda sus campos ORIGINALES + un log de cambios
// (campo, valor_nuevo, tiempo) de tamaño máximo 2p.
//   - En un BST cada nodo tiene p = 1 puntero entrante (su padre, o el
//     puntero externo raiz[t] si es la raíz)  =>  log de 2p = 2 entradas.
//   - Leer campo c en versión t: recorrer log de atrás hacia adelante,
//     primer cambio con tiempo <= t; si no hay, valor original.
//   - Escribir en la versión actual: si hay espacio en el log, agregar.
//     Si el log está lleno -> SPLIT: nodo nuevo con los valores más
//     recientes (log vacío) y se redirige el puntero entrante (el padre
//     escribe en SU log "desde t apunto al nuevo"; eso puede provocar
//     otro split hacia arriba -> cascada).
//   - Para saber quién me apunta (y redirigir) cada nodo guarda un
//     puntero inverso "padre", válido SOLO para la versión más reciente.
//
// PARCIAL: solo se modifica la última versión; se consulta cualquiera.
// Costos: O(1) amortizado de overhead por cambio (potencial Φ = Σ entradas
// usadas en logs). insertar/eliminar O(h), consultar versión t O(h).
// =====================================================================
#include <iostream>
#include <vector>
#include <set>
#include <climits>
#include <cstdlib>
using namespace std;

typedef int Llave;                 // cambia aquí el tipo de llave

const int P = 1;                   // punteros entrantes máximos por nodo
const int MAX_LOG = 2 * P;         // tamaño del log (2p)
const int INF_T = INT_MAX;         // "tiempo infinito" = versión más reciente

enum Campo { LLAVE, IZQ, DER };

struct NodoG;

struct Mod {
    Campo campo;
    Llave llave;       // se usa si campo == LLAVE
    NodoG* ptr;        // se usa si campo == IZQ / DER
    int tiempo;
};

struct NodoG {
    // valores originales
    Llave llave;
    NodoG* izq;
    NodoG* der;
    // log de modificaciones
    Mod log[MAX_LOG];
    int nlog;
    // puntero inverso (versión más reciente) para redirigir en el split
    NodoG* padre;
};

long long totalSplits = 0;

struct BSTGordo {
    vector<NodoG*> raiz;   // raiz[t] = raíz en la versión t (puntero externo)
    int actual = 0;        // última versión

    BSTGordo() { raiz.push_back(nullptr); }

    // ---------- lectura en versión t ----------
    NodoG* leerPtr(NodoG* n, Campo c, int t) {
        for (int i = n->nlog - 1; i >= 0; i--)
            if (n->log[i].campo == c && n->log[i].tiempo <= t) return n->log[i].ptr;
        return (c == IZQ) ? n->izq : n->der;
    }
    Llave leerLlave(NodoG* n, int t) {
        for (int i = n->nlog - 1; i >= 0; i--)
            if (n->log[i].campo == LLAVE && n->log[i].tiempo <= t) return n->log[i].llave;
        return n->llave;
    }

    NodoG* nuevoNodo(Llave x, NodoG* padre) {
        NodoG* n = new NodoG;
        n->llave = x; n->izq = nullptr; n->der = nullptr;
        n->nlog = 0; n->padre = padre;
        return n;
    }

    // ---------- escritura en la versión t (la actual) ----------
    // Devuelve el nodo que queda con la versión más reciente (n, o su copia si hubo split)
    NodoG* escribir(NodoG* n, Campo c, Llave k, NodoG* p, int t) {
        if (n->nlog < MAX_LOG) {
            n->log[n->nlog++] = Mod{c, k, p, t};
        } else {
            // SPLIT: nodo limpio con los valores más recientes + esta escritura
            totalSplits++;
            NodoG* m = new NodoG;
            m->llave = leerLlave(n, INF_T);
            m->izq   = leerPtr(n, IZQ, INF_T);
            m->der   = leerPtr(n, DER, INF_T);
            m->nlog  = 0;
            m->padre = n->padre;
            if (c == LLAVE) m->llave = k;
            else if (c == IZQ) m->izq = p;
            else m->der = p;

            // los hijos (versión actual) ahora cuelgan de m
            if (m->izq) m->izq->padre = m;
            if (m->der) m->der->padre = m;

            // redirigir el único puntero entrante (p = 1)
            if (m->padre == nullptr) {
                raiz[t] = m;                                   // era la raíz
            } else {
                NodoG* pa = m->padre;
                Campo lado = (leerPtr(pa, IZQ, INF_T) == n) ? IZQ : DER;
                escribir(pa, lado, 0, m, t);                   // puede hacer split en cascada
            }
            n = m;   // el nodo viejo n queda intacto para versiones < t
        }
        if (c != LLAVE && p != nullptr) p->padre = n;          // actualizar puntero inverso
        return n;
    }

    // cambia el puntero que apunta a "hijo viejo" desde "padre" por "nuevoHijo"
    void reemplazar(NodoG* padre, Campo lado, NodoG* nuevoHijo, int t) {
        if (padre == nullptr) {
            raiz[t] = nuevoHijo;
            if (nuevoHijo) nuevoHijo->padre = nullptr;
        } else {
            escribir(padre, lado, 0, nuevoHijo, t);
        }
    }

    Campo ladoDe(NodoG* hijo) {
        return (leerPtr(hijo->padre, IZQ, INF_T) == hijo) ? IZQ : DER;
    }

    // ---------- operaciones (sobre la ÚLTIMA versión) ----------
    int nuevaVersion() {
        actual++;
        raiz.push_back(raiz[actual - 1]);
        return actual;
    }

    int insertar(Llave x) {
        int t = nuevaVersion();
        if (raiz[t] == nullptr) { raiz[t] = nuevoNodo(x, nullptr); return t; }
        NodoG* cur = raiz[t];
        while (true) {
            Llave k = leerLlave(cur, t);
            if (x == k) return t;                          // ya existe
            Campo dir = (x < k) ? IZQ : DER;
            NodoG* sig = leerPtr(cur, dir, t);
            if (sig == nullptr) {
                escribir(cur, dir, 0, nuevoNodo(x, cur), t);
                return t;
            }
            cur = sig;
        }
    }

    int eliminar(Llave x) {
        int t = nuevaVersion();
        NodoG* z = raiz[t];
        while (z != nullptr && leerLlave(z, t) != x)
            z = leerPtr(z, (x < leerLlave(z, t)) ? IZQ : DER, t);
        if (z == nullptr) return t;                        // no estaba

        NodoG* zi = leerPtr(z, IZQ, t);
        NodoG* zd = leerPtr(z, DER, t);
        if (zi == nullptr || zd == nullptr) {
            NodoG* hijo = zi ? zi : zd;
            if (z->padre == nullptr) reemplazar(nullptr, IZQ, hijo, t);
            else reemplazar(z->padre, ladoDe(z), hijo, t);
            return t;
        }
        // dos hijos: copio la llave del sucesor en z y quito al sucesor
        NodoG* s = zd;
        while (leerPtr(s, IZQ, t) != nullptr) s = leerPtr(s, IZQ, t);
        Llave ks = leerLlave(s, t);
        escribir(z, LLAVE, ks, nullptr, t);                // puede hacer split de z
        // s->padre ya está actualizado aunque z se haya partido
        reemplazar(s->padre, ladoDe(s), leerPtr(s, DER, t), t);
        return t;
    }

    // ---------- consultas en CUALQUIER versión ----------
    bool buscar(int t, Llave x) {
        NodoG* n = raiz[t];
        while (n != nullptr) {
            Llave k = leerLlave(n, t);
            if (x == k) return true;
            n = leerPtr(n, (x < k) ? IZQ : DER, t);
        }
        return false;
    }

    void inordenRec(NodoG* n, int t, vector<Llave>& out) {
        if (!n) return;
        inordenRec(leerPtr(n, IZQ, t), t, out);
        out.push_back(leerLlave(n, t));
        inordenRec(leerPtr(n, DER, t), t, out);
    }
    vector<Llave> inorden(int t) { vector<Llave> out; inordenRec(raiz[t], t, out); return out; }

    void imprimir(int t) {
        cout << "v" << t << ": ";
        for (Llave x : inorden(t)) cout << x << " ";
        cout << "\n";
    }
};

int main() {
    BSTGordo T;
    for (int x : {50, 30, 70, 20, 40, 60, 80}) T.insertar(x);   // v1..v7
    T.eliminar(30);                                              // v8 (2 hijos)
    T.insertar(35);                                              // v9
    T.eliminar(50);                                              // v10 (raíz)

    for (int t = 0; t <= T.actual; t++) T.imprimir(t);
    cout << "splits hasta ahora: " << totalSplits << "\n";
    cout << "buscar(30) en v7=" << T.buscar(7, 30) << "  en v8=" << T.buscar(8, 30) << "\n";

    // --- prueba aleatoria contra std::set: todas las versiones ---
    srand(123);
    BSTGordo G;
    vector<set<Llave>> ref(1);
    long long splitsAntes = totalSplits;
    int OPS = 20000;
    for (int i = 0; i < OPS; i++) {
        Llave x = rand() % 500;
        set<Llave> s = ref.back();
        if (rand() % 3) { G.insertar(x); s.insert(x); }
        else            { G.eliminar(x); s.erase(x); }
        ref.push_back(s);
    }
    bool ok = true;
    for (int t = 0; t <= G.actual; t += 1)
        if (G.inorden(t) != vector<Llave>(ref[t].begin(), ref[t].end())) { ok = false; break; }
    cout << "prueba aleatoria (" << OPS << " versiones): " << (ok ? "OK" : "FALLO")
         << "  | splits por operación: " << (double)(totalSplits - splitsAntes) / OPS << " (O(1) amortizado)\n";
    return 0;
}
