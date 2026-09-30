// AVL
// casos LL LR RR RL
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cstdlib>
using namespace std;

struct NodoA {
    int key;
    int altura;
    NodoA* left;
    NodoA* right;
};

int h(NodoA* p) {
    if (p == nullptr) return 0;
    return p->altura;
}
void actualizar(NodoA* p) {
    int n1 = h(p->left);
    int n2 = h(p->right);
    if (n1 > n2) p->altura = 1 + n1;
    else p->altura = 1 + n2;
}
int balance(NodoA* p) { return h(p->left) - h(p->right); }

NodoA* rotarDer(NodoA* y) {
    NodoA* x = y->left;
    y->left = x->right;
    x->right = y;
    actualizar(y);
    actualizar(x);
    return x;
}
NodoA* rotarIzq(NodoA* x) {
    NodoA* y = x->right;
    x->right = y->left;
    y->left = x;
    actualizar(x);
    actualizar(y);
    return y;
}
NodoA* rebalancear(NodoA* p) {
    actualizar(p);
    int b = balance(p);
    if (b > 1) {
        if (balance(p->left) < 0) p->left = rotarIzq(p->left);  // LR
        return rotarDer(p);
    }
    if (b < -1) {
        if (balance(p->right) > 0) p->right = rotarDer(p->right);  // RL
        return rotarIzq(p);
    }
    return p;
}

NodoA* insertar(NodoA* p, int x) {
    if (p == nullptr) {
        NodoA* q = new NodoA;
        q->key = x; q->altura = 1;
        q->left = nullptr; q->right = nullptr;
        return q;
    }
    if (x < p->key) p->left = insertar(p->left, x);
    else if (p->key < x) p->right = insertar(p->right, x);
    else return p;
    return rebalancear(p);
}

NodoA* quitarMin(NodoA* p, NodoA*& aux) {
    if (p->left == nullptr) {
        aux = p;
        return p->right;
    }
    p->left = quitarMin(p->left, aux);
    return rebalancear(p);
}

NodoA* eliminar(NodoA* p, int x) {
    if (p == nullptr) return nullptr;
    if (x < p->key) {
        p->left = eliminar(p->left, x);
    }
    else if (p->key < x) {
        p->right = eliminar(p->right, x);
    }
    else {
        NodoA* l = p->left;
        NodoA* r = p->right;
        delete p;
        if (r == nullptr) return l;
        NodoA* m;
        r = quitarMin(r, m);
        m->left = l;
        m->right = r;
        return rebalancear(m);
    }
    return rebalancear(p);
}

bool buscar(NodoA* p, int x) {
    while (p != nullptr) {
        if (x == p->key) return true;
        if (x < p->key) p = p->left;
        else p = p->right;
    }
    return false;
}

void inorden(NodoA* p, vector<int>& res) {
    if (p == nullptr) return;
    inorden(p->left, res);
    res.push_back(p->key);
    inorden(p->right, res);
}
bool esAVL(NodoA* p) {
    if (p == nullptr) return true;
    int b = balance(p);
    if (b > 1 || b < -1) return false;
    int n1 = h(p->left), n2 = h(p->right);
    int mayor = n1;
    if (n2 > mayor) mayor = n2;
    if (p->altura != 1 + mayor) return false;
    return esAVL(p->left) && esAVL(p->right);
}

int main() {
    NodoA* r = nullptr;
    for (int i = 1; i <= 1023; i++) r = insertar(r, i);  // ordenado
    cout << "1023 inserts ordenados -> altura " << h(r) << " (un BST normal tendría 1023)\n";
    return 0;
}
