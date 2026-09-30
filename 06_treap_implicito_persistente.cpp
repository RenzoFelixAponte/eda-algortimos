// treap implicito persistente
// split y merge copiando el camino, concat de versiones = confluente
// el merge escoge la raiz al azar segun tamanos (asi se puede hacer concat(v,v))
#include <iostream>
#include <vector>
#include <random>
using namespace std;

typedef long long ll;
mt19937 rng(2026);

struct NodoT {
    ll val;
    ll tam;
    ll suma;
    NodoT* left;
    NodoT* right;
};

ll tam(NodoT* p) {
    if (p == nullptr) return 0;
    return p->tam;
}
ll suma(NodoT* p) {
    if (!p) return 0;
    return p->suma;
}

NodoT* actualizar(NodoT* p) {
    p->tam = 1 + tam(p->left) + tam(p->right);
    p->suma = p->val + suma(p->left) + suma(p->right);
    return p;
}
NodoT* copiar(NodoT* p) {
    NodoT* q = new NodoT(*p);
    return q;
}
NodoT* hoja(ll x) {
    NodoT* p = new NodoT;
    p->val = x; p->tam = 1; p->suma = x;
    p->left = nullptr; p->right = nullptr;
    return p;
}

// a = primeros k, b = resto
void split(NodoT* p, ll k, NodoT*& a, NodoT*& b) {
    if (p == nullptr) {
        a = nullptr;
        b = nullptr;
        return;
    }
    NodoT* q = copiar(p);
    if (tam(p->left) >= k) {
        split(p->left, k, a, q->left);
        b = actualizar(q);
    }
    else {
        split(p->right, k - tam(p->left) - 1, q->right, b);
        a = actualizar(q);
    }
}

NodoT* merge(NodoT* a, NodoT* b) {
    if (a == nullptr) return b;
    if (b == nullptr) return a;
    uniform_int_distribution<long long> dist(0, tam(a) + tam(b) - 1);
    if (dist(rng) < tam(a)) {
        NodoT* q = copiar(a);
        q->right = merge(a->right, b);
        return actualizar(q);
    } else {
        NodoT* q = copiar(b);
        q->left = merge(a, b->left);
        return actualizar(q);
    }
}

struct SecuenciaPersistente {
    vector<NodoT*> raiz;
    SecuenciaPersistente() { raiz.push_back(nullptr); }

    int nueva(NodoT* r) {
        raiz.push_back(r);
        return (int)raiz.size() - 1;
    }

    int desdeVector(const vector<ll>& a) {
        NodoT* r = nullptr;
        for (int i = 0; i < (int)a.size(); i++) r = merge(r, hoja(a[i]));
        return nueva(r);
    }

    int insertar(int v, ll pos, ll x) {
        NodoT *a, *b;
        split(raiz[v], pos, a, b);
        NodoT* aux = merge(a, hoja(x));
        aux = merge(aux, b);
        return nueva(aux);
    }
    int borrar(int v, ll pos) {
        NodoT *a, *b, *m, *c;
        split(raiz[v], pos, a, b);
        split(b, 1, m, c);
        return nueva(merge(a, c));
    }
    int asignar(int v, ll pos, ll x) {
        NodoT *a, *b, *m, *c;
        split(raiz[v], pos, a, b);
        split(b, 1, m, c);
        NodoT* aux = merge(a, hoja(x));
        return nueva(merge(aux, c));
    }
    int concat(int v1, int v2) { return nueva(merge(raiz[v1], raiz[v2])); }
    int subsecuencia(int v, ll l, ll r) {
        NodoT *a, *b, *m, *c;
        split(raiz[v], l, a, b);
        split(b, r - l + 1, m, c);
        return nueva(m);
    }

    ll obtener(int v, ll pos) {
        NodoT* p = raiz[v];
        while (true) {
            ll n1 = tam(p->left);
            if (pos == n1) return p->val;
            if (pos < n1) p = p->left;
            else {
                pos = pos - (n1 + 1);
                p = p->right;
            }
        }
    }
    ll sumaPrefijo(NodoT* p, ll k) {
        ll s = 0;
        while (p != nullptr && k > 0) {
            if (tam(p->left) >= k) {
                p = p->left;
            } else {
                s += suma(p->left) + p->val;
                k -= tam(p->left) + 1;
                p = p->right;
            }
        }
        return s;
    }
    ll sumaRango(int v, ll l, ll r) {
        ll n1 = sumaPrefijo(raiz[v], r + 1);
        ll n2 = sumaPrefijo(raiz[v], l);
        return n1 - n2;
    }
    ll tamano(int v) { return tam(raiz[v]); }

    void imprimirRec(NodoT* p) {
        if (p == nullptr) return;
        imprimirRec(p->left);
        cout << p->val << " ";
        imprimirRec(p->right);
    }
    void imprimir(int v) {
        cout << "v" << v << ": ";
        imprimirRec(raiz[v]);
        cout << "\n";
    }
};

int main() {
    SecuenciaPersistente S;
    int v1 = S.desdeVector({1, 2, 3, 4, 5});
    int v2 = S.insertar(v1, 2, 99);
    int v3 = S.borrar(v1, 0);
    int v4 = S.asignar(v2, 4, -7);
    int v5 = S.concat(v3, v4);  // confluente
    int v6 = S.subsecuencia(v5, 3, 6);

    int vs[6] = {v1, v2, v3, v4, v5, v6};
    for (int i = 0; i < 6; i++) S.imprimir(vs[i]);
    cout << "suma v5 [2,6] = " << S.sumaRango(v5, 2, 6) << " (4+5+1+2+99=111)\n";

    // duplicar muchas veces
    int v = S.desdeVector({1, 2, 3});
    for (int u = 0; u < 40; u++) v = S.concat(v, v);
    cout << "tras 40 auto-concats: tam = " << S.tamano(v) << " (= 3 * 2^40)"
         << ", elemento en pos 10^12 = " << S.obtener(v, 1000000000000LL) << "\n";
    return 0;
}
