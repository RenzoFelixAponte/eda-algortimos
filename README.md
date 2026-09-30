# Plantillas EDA (Estructuras de Datos Avanzadas)

Cada `.cpp` se compila solo y trae un `main` con un ejemplo. Casi todos tienen además una prueba aleatoria contra `std::set` o fuerza bruta. Los nombres y el estilo (`struct`, punteros, español) siguen los apuntes del curso.

```bash
g++ -std=c++17 -O2 -o bin/03.exe 03_bst_nodos_gordos.cpp && ./bin/03.exe
```

Para compilar y probar todo:

```bash
mkdir -p bin && for f in [0-9]*.cpp; do g++ -std=c++17 -O2 -o bin/${f%.cpp}.exe $f && echo "== $f" && ./bin/${f%.cpp}.exe; done
```

## Índice por tema

### Persistencia (semana 3)

| # | Archivo | Técnica | Nivel | Costo |
|---|---------|---------|-------|-------|
| 01 | `01_stack_persistente.cpp` | funcional | total (+ confluente con `concat`) | push/pop O(1), concat O(\|S1\|) |
| 02 | `02_bst_path_copying.cpp` | path copying | total | O(h) tiempo y espacio |
| 03 | `03_bst_nodos_gordos.cpp` | nodos gordos (DSST): log de 2p + split | **parcial** | O(1) amortizado de overhead |
| 04 | `04_segment_tree_persistente.cpp` | path copying | total / arreglo persistente | O(lg n) |
| 05 | `05_segtree_persistente_aplicaciones.cpp` | una versión por prefijo | — | distintos en [l,r], k-ésimo menor, cuántos ≤ x |
| 06 | `06_treap_implicito_persistente.cpp` | path copying + split/merge | **confluente** | O(lg n) esperado |
| 07 | `07_leftist_heap_persistente.cpp` | path copying (espina derecha) | **confluente** | O(lg n) |

### Montículos (semana 2)

| # | Archivo | Qué tiene |
|---|---------|-----------|
| 08 | `08_heap_binario.cpp` | heapify, buildHeap O(n) vs buildHeapInsert Θ(n lg n) (ejercicio del examen) |
| 09 | `09_heap_binomial.cpp` | unir, insertar O(1) amortizado (ej. 6), extraerMin, decreaseKey |
| 10 | `10_heap_fibonacci.cpp` | insert, union, extractMin + consolidate, decreaseKey con cut y cascading cut, delete |

### Árboles y Segment Tree

| # | Archivo | Qué tiene |
|---|---------|-----------|
| 11 | `11_segment_tree_lazy.cpp` | segment tree con suma en rango y lazy propagation |
| 17 | `17_avl.cpp` | BST balanceado (LL, LR, RR, RL): insertar y eliminar en O(lg n) |
| 18 | `18_splay_tree.cpp` | zig, zig-zig, zig-zag; muestra acceso secuencial y working set |

### Orthogonal Range Search (semanas 4 y 5)

| # | Archivo | Consulta | Espacio |
|---|---------|----------|---------|
| 12 | `12_range_search_1d.cpp` | 1D: arreglo + binaria, o BST con v_split: O(lg n + k) | O(n) |
| 13 | `13_range_tree_2d.cpp` | Range Tree 2D: O(lg² n + k) | O(n lg n) |
| 14 | `14_fractional_cascading.cpp` | k listas en O(k + lg n) | O(total) |
| 15 | `15_layered_range_tree.cpp` | Layered Range Tree 2D: O(lg n + k), también dominancia | O(n lg n) |
| 16 | `16_range_tree_3d.cpp` | Range Tree 3D: O(lg² n + k) | O(n lg² n) |

### Dynamic Optimality (semana 6)

| # | Archivo | Qué tiene |
|---|---------|-----------|
| 18 | `18_splay_tree.cpp` | (ver arriba) |
| 19 | `19_greedy_satisfaccion_arboral.cpp` | vista geométrica, verificador de conjunto arboralmente satisfecho, Greedy y OPT por fuerza bruta (ej. 10 y 15) |

### Retroactividad

| # | Archivo | Qué tiene |
|---|---------|-----------|
| 20 | `20_cola_retroactiva.cpp` | cola totalmente retroactiva con 2 árboles de estadística de orden, O(lg m) (ej. 3 y 8) |
| 21 | `21_pq_retroactiva.cpp` | cola de prioridad parcialmente retroactiva: puentes y las 4 reglas (ej. 13) |

## Qué usar según el enunciado

- **"Parcial" / "nodos gordos" / "DSST":** usa 03. Cambia `P` por el número de punteros **entrantes** por nodo.
- **"Total" / "actualizar cualquier versión":** usa path copying (02, 04, 06).
- **"Confluente" / "combinar dos versiones":** usa 06 (concat) o 07 (unir).
- **Consultas sobre [l, r] de un arreglo fijo:** usa 05 (una versión por prefijo).
- **Puntos en un rectángulo:** si el enunciado pide O(lg² n + k), usa 13. Si pide O(lg n + k), usa 15. Para una caja 3D usa 16.
- **Buscar lo mismo en muchas listas ordenadas:** usa 14.
- **Cola de prioridad con decrease-key barato:** usa 10. Si además debe ser persistente, usa 07.

## Cómo modificarlas

- **Otra operación en el Segment Tree:** cambia `combinar` y `NEUTRO` (04) o la suma de `t[]` (11).
- **Otro tipo de llave:** cambia `typedef int Llave` (o el parámetro del template en 01 y 02).
- **Contar cuánto trabaja una operación:** usa los contadores globales (`nodosCreados`, `totalSplits`, `swaps`, `links`, `rotaciones`).

## Notas para el examen

- El contraejemplo de Build-Heap vs Build-Heap-Insert depende del tipo de heap. En **min-heap** sirve `[3,2,1]` (da `[1,2,3]` vs `[1,3,2]`). En **max-heap** (CLRS) sirve `[1,2,3]`. `[3,1,2]` no sirve en min-heap: los dos métodos dan `[1,3,2]`.
- Path copying copia un nodo por nivel del camino raíz → cambio. Con nodos gordos, Φ = Σ(entradas usadas) y el costo amortizado queda en O(p) − p = O(1).
- Las consultas **nunca** suben el nivel de persistencia. Solo cuentan las modificaciones.
- El Fibonacci Heap **no** es persistente tal cual, porque modifica punteros en su lugar.
- En la PQ retroactiva, cada cambio en el pasado cambia Q_now en **exactamente un** elemento.
