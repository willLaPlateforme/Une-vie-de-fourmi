# Une-vie-de-fourmi

> *Simuler le déplacement optimal de fourmis dans une fourmilière, de l'entrée jusqu'au dortoir, en un minimum d'étapes.*

---

## 📋 Sommaire

- [Le projet](#-le-projet)
- [Prérequis](#-prérequis)
- [Installation](#-installation)
- [Lancer le programme](#-lancer-le-programme)
- [Les fourmilières](#-les-fourmilières)
- [Les algorithmes](#-les-algorithmes)
- [Architecture du code](#-architecture-du-code)
- [Exemple de sortie](#-exemple-de-sortie)
- [Auteurs](#-auteurs)

---

## 🏠 Le projet

Une colonie de **F fourmis** se retrouve chaque nuit dans le **vestibule** (`Sv`) de leur fourmilière. Elles doivent toutes rejoindre le **dortoir** (`Sd`) en passant par des salles intermédiaires reliées par des tunnels.

**L'objectif :** déplacer toutes les fourmis en un **minimum d'étapes**.

### Règles

| Règle | Détail |
|-------|--------|
| Capacité des salles | Chaque salle intermédiaire n'accepte qu'**1 fourmi à la fois** (sauf si spécifié) |
| Déplacement | Une fourmi avance d'**1 salle par étape** |
| Attente | Si la salle suivante est pleine, la fourmi **attend** |
| Tunnels | Traversés **instantanément** (bidirectionnels) |
| Vestibule / Dortoir | Capacité **illimitée** |

---

## 💻 Prérequis

- **Compilateur C++17** : `g++` (GCC 7+ ou Clang 5+)
- **Windows** : installer [MSYS2](https://www.msys2.org/) avec GCC, ou utiliser Git Bash
- **Linux / Mac** : GCC généralement déjà installé

Vérifier l'installation :
```bash
g++ --version
```

---

## 📦 Installation

```bash
# 1. Cloner le repository
git clone https://github.com/prenom-nom/uneviedefourmi.git

# 2. Aller dans le dossier
cd uneviedefourmi

# 3. Compiler
g++ -std=c++17 -O2 -o ants Main.cpp Ants.cpp Bfs.cpp Dijkstra.cpp Dfs.cpp
```

> ⚠️ **Attention à la casse** : les fichiers s'appellent `Ants.hpp`, `Main.cpp`, etc. avec une majuscule. Sur Linux, les `#include` doivent correspondre exactement.

---

## 🚀 Lancer le programme

```bash
# Windows
.\ants.exe

# Linux / Mac
./ants
```

Le programme affiche un **menu interactif** :

```
  ╔══════════════════════════════════════════╗
  ║         🐜  Une vie de fourmi  🐜        ║
  ╚══════════════════════════════════════════╝

  Choisissez un algorithme :

  [1] BFS       — tous les chemins courts, fourmis en parallele
  [2] Dijkstra  — chemin le plus court, file indienne
  [3] DFS       — exploration en profondeur + backtracking
  [0] Quitter
```

1. **Choisir un algorithme** (1, 2 ou 3)
2. **Choisir une fourmilière** parmi les 9 disponibles (ou toutes)
3. **Appuyer sur Entrée** pour avancer étape par étape
4. Taper **`q` + Entrée** pour quitter la simulation en cours

> 💡 Les couleurs s'affichent dans le terminal intégré de **VS Code** et dans **Git Bash**. Elles peuvent ne pas fonctionner dans `cmd.exe` classique.

---

## 📂 Les fourmilières

9 fourmilières sont incluses dans le projet :

| Fichier | Fourmis | Description |
|---------|---------|-------------|
| `fourmiliere_zero.txt` | 2 | Cas simple — 2 chemins parallèles |
| `fourmiliere_un.txt` | 5 | Chemin unique en ligne droite |
| `fourmiliere_deux.txt` | 5 | Chemin avec tunnel retour `Sd → Sv` |
| `fourmiliere_trois.txt` | 5 | Chemin avec bifurcation |
| `fourmiliere_quatre.txt` | 10 | Salles à capacité 2 |
| `fourmiliere_cinq.txt` | 50 | Réseau complexe, plusieurs couloirs |
| `fourmiliere_3D.txt` | 50 | Graphe dense avec capacités variées |
| `La_hormiguera_de_la_muerte.txt` | 30 | Graphe très connecté (graphe complet) |
| `salle_d_at-ant.txt` | 100 | 4 couloirs parallèles, 100 fourmis |

### Format d'un fichier

```
f=3          ← nombre de fourmis (accepte aussi F=)
S1 { 2 }     ← salle S1 avec capacité 2
S2           ← salle S2 avec capacité 1 (par défaut)
Sv - S1      ← tunnel entre le vestibule et S1
S1 - S2      ← tunnel entre S1 et S2
S2 - Sd      ← tunnel entre S2 et le dortoir
```

### Créer sa propre fourmilière

Créer un fichier `.txt` en respectant ce format, le placer dans le dossier du projet, et l'ajouter dans la liste `fichiers` dans `Main.cpp` :

```cpp
std::vector<std::string> fichiers = {
    "fourmiliere_zero.txt",
    // ...
    "ma_fourmiliere.txt"   // ← ajouter ici
};
```

---

## 🧠 Les algorithmes

### 1. BFS — Breadth-First Search (`Bfs.cpp`)

Explore le graphe **niveau par niveau** depuis `Sv`. La file contient des chemins entiers. Tous les chemins possibles sont trouvés, puis les fourmis sont **réparties en parallèle** sur les meilleurs.

```
Sv → [S1, S2]       ← niveau 1
S1 → [Sd]           ← niveau 2 (chemin 1 trouvé : Sv→S1→Sd)
S2 → [Sd]           ← niveau 2 (chemin 2 trouvé : Sv→S2→Sd)
```

✅ **Efficace** quand plusieurs chemins parallèles existent.

---

### 2. Dijkstra (`Dijkstra.cpp`)

Utilise une **file de priorité (min-heap)** pour toujours traiter la salle la moins coûteuse en premier. Chaque tunnel a un coût de 1. Trouve **tous les chemins de longueur minimale**, puis répartit les fourmis dessus.

```
dist[Sv] = 0
dist[S1] = 1  (Sv → S1)
dist[Sd] = 2  (Sv → S1 → Sd)  ← chemin optimal
```

✅ **Efficace** quand un couloir court direct existe.

---

### 3. DFS — Depth-First Search (`Dfs.cpp`)

Explore le graphe **en profondeur** via une fonction récursive. Plonge jusqu'au dortoir ou un cul-de-sac, puis **revient en arrière** (backtracking) pour essayer d'autres voisins.

```
Sv → S1 → S2 → S3 → Sd   ✓ (chemin trouvé)
          ↑ backtrack
       → S4 → Sd          ✓ (autre chemin)
```

✅ **Logique récursive** très claire, résultats similaires à BFS.

---

### Comparaison des résultats

| Fourmilière | Fourmis | BFS | Dijkstra | DFS |
|-------------|---------|-----|----------|-----|
| zero | 2 | 2 | 2 | 2 |
| un | 5 | 7 | 7 | 7 |
| deux | 5 | 4 | **1** | 4 |
| trois | 5 | 7 | 7 | 7 |
| quatre | 10 | 10 | 10 | 10 |
| cinq | 50 | **13** | 17 | **13** |
| 3D | 50 | 48 | **28** | 34 |
| hormiguera | 30 | **9** | **9** | **9** |
| salle_d_at | 100 | **32** | **30** | **30** |

> Les valeurs en **gras** indiquent le meilleur résultat pour chaque fourmilière.

---

## 🗂️ Architecture du code

```
uneviedefourmi/
├── Ants.hpp          ← déclarations : Room, Ant, Move, classe Anthill
├── Ants.cpp          ← construction du graphe, simulation, parser
├── Bfs.cpp           ← algorithme BFS
├── Dijkstra.cpp      ← algorithme Dijkstra
├── Dfs.cpp           ← algorithme DFS
├── Main.cpp          ← interface interactive, menu, affichage
├── fourmiliere_zero.txt
├── fourmiliere_un.txt
├── fourmiliere_deux.txt
├── fourmiliere_trois.txt
├── fourmiliere_quatre.txt
├── fourmiliere_cinq.txt
├── fourmiliere_3D.txt
├── La_hormiguera_de_la_muerte.txt
└── salle_d_at-ant.txt
```

### Flux d'exécution

```
parseFile()          → construit l'objet Anthill depuis le fichier .txt
    ↓
simulate()           → BFS  ┐
simulateDijkstra()   → Dijk ├─ trouvent les chemins
simulateDFS()        → DFS  ┘
    ↓
selectBestPaths()    → filtre les chemins trop longs (longueur_min + 2 max)
    ↓
runSimulation()      → répartit les fourmis, simule étape par étape
```

---

## 📺 Exemple de sortie

```
[BFS] Chemins trouves : 2
  Chemin 1 : Sv -> S1 -> Sd
  Chemin 2 : Sv -> S2 -> Sd

+++ E1 +++
f1 - Sv - S1
f2 - Sv - S2
+++ E2 +++
f1 - S1 - Sd
f2 - S2 - Sd
f3 - Sv - S1
+++ E3 +++
f3 - S1 - Sd

=> 3 etape(s) [BFS] pour 3 fourmi(s)
```

---

## 👥 Auteurs

| Rôle | Responsabilité |
|------|---------------|
| **Ilies chapuis** | Algorithme BFS (`Bfs.cpp`) + architecture |
| **Melvin Vincent** | Algorithme Dijkstra (`Dijkstra.cpp`) + parser |
| **Guillaume Ciampa** | Algorithme DFS (`Dfs.cpp`) + présentation |

---

*Projet réalisé dans le cadre de la formation C++ — La Plateforme 2025*
