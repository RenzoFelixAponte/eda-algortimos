// range search 1D
// con arreglo ordenado + binaria, y con BST balanceado (nodo split)
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// arreglo ordenado
struct Rango1DArreglo {
    vector<int> a;
    Rango1DArreglo(vector<int> v) {
        a = v;
        sort(a.begin(), a.end());
    }
    int contar(int l, int r) {
        int n1 = upper_bound(a.begin(), a.end(), r) - a.begin();
        int n2 = lower_bound(a.begin(), a.end(), l) - a.begin();
        return n1 - n2;
    }
    bool existe(int l, int r) { return contar(l, r) > 0; }
    vector<int> reportar(int l, int r) {
        vector<int> res;
        int i = lower_bound(a.begin(), a.end(), l) - a.begin();
        while (i < (int)a.size() && a[i] <= r) {
            res.push_back(a[i]);
            i++;
        }
        return res;
    }
};

// BST
struct Nodo {
    int key;
    int tam;
    Nodo* left;
    Nodo* right;
};
int tam(Nodo* p) {
    if (p == nullptr) return 0;
    return p->tam;
}

struct Rango1DBST {
    Nodo* raiz;
    // la mediana va de raiz
    Nodo* build(const vector<int>& a, int l, int r) {
        if (l > r) return nullptr;
        int m = (l + r) / 2;
        Nodo* p = new Nodo;
        p->key = a[m];
        p->left = build(a, l, m - 1);
        p->right = build(a, m + 1, r);
        p->tam = 1 + tam(p->left) + tam(p->right);
        return p;
    }
    Rango1DBST(vector<int> v) {
        sort(v.begin(), v.end());
        raiz = build(v, 0, (int)v.size() - 1);
    }

    Nodo* split(int l, int r) {
        Nodo* p = raiz;
        while (p != nullptr && (r < p->key || p->key < l)) {
            if (r < p->key) p = p->left;
            else p = p->right;
        }
        return p;
    }
    void reportarTodo(Nodo* p, vector<int>& res) {
        if (p == nullptr) return;
        reportarTodo(p->left, res);
        res.push_back(p->key);
        reportarTodo(p->right, res);
    }

    vector<int> reportar(int l, int r) {
        vector<int> res;
        Nodo* s = split(l, r);
        if (s == nullptr) return res;
        // lado izquierdo
        vector<int> izq;
        Nodo* p = s->left;
        while (p != nullptr) {
            if (l <= p->key) {
                vector<int> aux;
                reportarTodo(p->right, aux);
                aux.insert(aux.begin(), p->key);
                izq.insert(izq.begin(), aux.begin(), aux.end());
                p = p->left;
            }
            else p = p->right;
        }
        res = izq;
        res.push_back(s->key);
        // lado derecho
        p = s->right;
        while (p != nullptr) {
            if (p->key <= r) {
                reportarTodo(p->left, res);
                res.push_back(p->key);
                p = p->right;
            } else {
                p = p->left;
            }
        }
        return res;
    }

    int contar(int l, int r) {
        Nodo* s = split(l, r);
        if (s == nullptr) return 0;
        int c = 1;
        Nodo* p = s->left;
        while (p) {
            if (l <= p->key) {
                c = c + 1 + tam(p->right);
                p = p->left;
            }
            else p = p->right;
        }
        Nodo* q = s->right;
        while (q) {
            if (q->key <= r) {
                c = c + 1 + tam(q->left);
                q = q->right;
            }
            else q = q->left;
        }
        return c;
    }
};

int main() {
    vector<int> pts = {2, 6, 7, 23, 42, 52, 68, 71};
    Rango1DArreglo A(pts);
    Rango1DBST B(pts);

    cout << "raíz del BST = " << B.raiz->key << "\n";
    cout << "split de [5, 50] = " << B.split(5, 50)->key << "\n";
    cout << "[5, 50] -> ";
    vector<int> res = B.reportar(5, 50);
    for (int i = 0; i < (int)res.size(); i++) cout << res[i] << " ";
    cout << "| conteo BST=" << B.contar(5, 50) << " arreglo=" << A.contar(5, 50) << "\n";
    return 0;
}
