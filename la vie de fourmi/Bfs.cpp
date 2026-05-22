// ═══════════════════════════════════════════════════════════
// bfs.cpp — Algorithme BFS (Breadth-First Search)
//
// Principe : explorer le graphe niveau par niveau depuis Sv.
// La file contient des chemins entiers. Les chemins les plus
// courts sont trouvés en premier. On sélectionne ensuite les
// K meilleurs pour répartir les fourmis en parallèle.
// ═══════════════════════════════════════════════════════════

#include "ants.hpp"
#include <queue>
#include <algorithm>
#include <iostream>

std::vector<std::vector<std::string>> Anthill::findAllPathsBFS() const
{
    std::vector<std::vector<std::string>> resultats;

    // La file contient des chemins entiers, pas juste des noeuds
    std::queue<std::vector<std::string>> file;
    file.push({ vestibule });

    while (!file.empty()) {
        std::vector<std::string> chemin = file.front();
        file.pop();

        std::string actuel = chemin.back();

        if (actuel == dortoir) {
            resultats.push_back(chemin);
            continue;
        }

        auto it = adjacency.find(actuel);
        if (it == adjacency.end()) continue;

        for (const std::string& voisin : it->second) {
            // Évite les cycles
            bool dejaVisite = false;
            for (const std::string& n : chemin)
                if (n == voisin) { dejaVisite = true; break; }

            if (!dejaVisite) {
                std::vector<std::string> nouveauChemin = chemin;
                nouveauChemin.push_back(voisin);
                file.push(nouveauChemin);
            }
        }
    }

    return resultats;
}

void Anthill::simulate()
{
    auto chemins = findAllPathsBFS();

    if (chemins.empty())
        throw std::runtime_error("BFS : aucun chemin trouvé !");

    std::cout << "[BFS] Chemins trouvés : " << chemins.size() << "\n";
    for (size_t i = 0; i < chemins.size(); i++) {
        std::cout << "  Chemin " << i + 1 << " : ";
        for (size_t j = 0; j < chemins[i].size(); j++) {
            std::cout << chemins[i][j];
            if (j + 1 < chemins[i].size()) std::cout << " -> ";
        }
        std::cout << "\n";
    }

    runSimulation(chemins, "BFS");
}