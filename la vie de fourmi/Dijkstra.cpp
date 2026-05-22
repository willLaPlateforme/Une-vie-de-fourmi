// ═══════════════════════════════════════════════════════════
// dijkstra.cpp — Algorithme de Dijkstra (multi-chemins)
//
// Principe : on utilise Dijkstra pour trouver le chemin le
// plus court, puis on l'étend en cherchant TOUS les chemins
// de même longueur minimale (ou longueur_min + 1 max).
// On répartit ensuite les fourmis sur ces chemins en parallèle
// pour maximiser le débit, comme BFS le fait.
// ═══════════════════════════════════════════════════════════

#include "ants.hpp"
#include <queue>
#include <algorithm>
#include <iostream>
#include <stdexcept>

// ── Étape 1 : Dijkstra classique pour trouver la distance minimale ──

static std::map<std::string, int> dijkstraDist(
    const std::map<std::string, std::vector<std::string>>& adjacency,
    const std::string& start)
{
    std::map<std::string, int> dist;
    for (auto& [n, _] : adjacency) dist[n] = INT_MAX;
    dist[start] = 0;

    std::priority_queue<
        std::pair<int, std::string>,
        std::vector<std::pair<int, std::string>>,
        std::greater<>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        auto it = adjacency.find(u);
        if (it == adjacency.end()) continue;
        for (auto& v : it->second) {
            int nd = dist[u] + 1;
            if (nd < dist[v]) {
                dist[v] = nd;
                pq.push({nd, v});
            }
        }
    }
    return dist;
}

// ── Étape 2 : BFS guidé — ne garde que les chemins dont chaque salle
//    est sur un chemin optimal (dist[u] + 1 == dist[v]) ──────────────

static std::vector<std::vector<std::string>> dijkstraAllShortestPaths(
    const std::map<std::string, std::vector<std::string>>& adjacency,
    const std::map<std::string, int>& dist,
    const std::string& start,
    const std::string& end,
    size_t longueurMin)
{
    std::vector<std::vector<std::string>> result;
    std::queue<std::vector<std::string>> q;
    q.push({start});

    // On autorise jusqu'à longueurMin + 1 tunnels (marge d'1 salle)
    const size_t LIMITE = longueurMin + 1 + 1;

    while (!q.empty()) {
        auto path = q.front(); q.pop();
        const std::string& cur = path.back();

        if (cur == end) { result.push_back(path); continue; }
        if (path.size() >= LIMITE) continue;

        auto it = adjacency.find(cur);
        if (it == adjacency.end()) continue;

        for (auto& v : it->second) {
            // Garder uniquement les arcs qui avancent vers le dortoir
            // (dist[v] == dist[cur] + 1) et éviter les cycles
            auto dv = dist.find(v);
            auto dc = dist.find(cur);
            if (dv == dist.end() || dc == dist.end()) continue;
            if (dv->second != dc->second + 1) continue;
            if (std::find(path.begin(), path.end(), v) != path.end()) continue;

            auto np = path; np.push_back(v);
            q.push(np);
        }
    }

    // Trier par longueur croissante
    std::sort(result.begin(), result.end(),
        [](const auto& a, const auto& b){ return a.size() < b.size(); });

    return result;
}

// ── Interface publique ───────────────────────────────────────────────

std::vector<std::string> Anthill::findPathDijkstra() const {
    // Garder pour compatibilité — retourne juste le plus court
    auto dist = dijkstraDist(adjacency, vestibule);
    if (dist[dortoir] == INT_MAX)
        throw std::runtime_error("Dijkstra : aucun chemin trouve !");

    std::map<std::string, std::string> pred;
    // On recalcule avec pred pour la reconstruction
    for (auto& [name, _] : adjacency) {
        auto it = adjacency.find(name);
        if (it == adjacency.end()) continue;
        for (auto& v : it->second) {
            if (dist.count(name) && dist.count(v) &&
                dist[name] != INT_MAX &&
                dist[v] == dist[name] + 1)
            {
                if (!pred.count(v))
                    pred[v] = name;
            }
        }
    }

    std::vector<std::string> chemin;
    std::string courant = dortoir;
    while (courant != vestibule) {
        chemin.push_back(courant);
        courant = pred[courant];
    }
    chemin.push_back(vestibule);
    std::reverse(chemin.begin(), chemin.end());
    return chemin;
}

void Anthill::simulateDijkstra() {
    // 1. Calculer les distances depuis Sv
    auto dist = dijkstraDist(adjacency, vestibule);
    if (dist[dortoir] == INT_MAX)
        throw std::runtime_error("Dijkstra : aucun chemin trouve !");

    size_t longueurMin = (size_t)dist[dortoir];

    // 2. Trouver TOUS les chemins optimaux (ou quasi-optimaux)
    auto tous = dijkstraAllShortestPaths(adjacency, dist,
                                          vestibule, dortoir, longueurMin);
    if (tous.empty())
        throw std::runtime_error("Dijkstra : reconstruction echouee !");

    // 3. Sélectionner les meilleurs (selectBestPaths = marge+limite)
    auto chemins = selectBestPaths(tous);

    std::cout << "[Dijkstra] Distance minimale : " << longueurMin
              << " tunnel(s) | " << tous.size() << " chemin(s) trouve(s), "
              << chemins.size() << " selectionne(s) :\n";
    for (size_t i = 0; i < chemins.size(); i++) {
        std::cout << "  Chemin " << i+1 << " : ";
        for (size_t j = 0; j < chemins[i].size(); j++) {
            std::cout << chemins[i][j];
            if (j+1 < chemins[i].size()) std::cout << " -> ";
        }
        std::cout << "\n";
    }

    // 4. Simuler avec tous ces chemins en parallèle
    runSimulation(chemins, "Dijkstra");
}