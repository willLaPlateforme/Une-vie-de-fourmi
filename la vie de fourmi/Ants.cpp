// ═══════════════════════════════════════════════════════════
// ants.cpp — Construction, simulation commune, parser
// Les algorithmes sont dans bfs.cpp, dijkstra.cpp, dfs.cpp
// ═══════════════════════════════════════════════════════════

#include "ants.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdexcept>

// ─── Construction ─────────────────────────────────

Anthill::Anthill(int nbAnts) : nbAnts(nbAnts) {
    rooms["Sv"] = Room("Sv", INT_MAX, true,  false);
    rooms["Sd"] = Room("Sd", INT_MAX, false, true);
    adjacency["Sv"] = {};
    adjacency["Sd"] = {};
    vestibule = "Sv";
    dortoir   = "Sd";
}

void Anthill::addRoom(const Room& room) {
    rooms[room.name] = room;
    if (adjacency.find(room.name) == adjacency.end())
        adjacency[room.name] = {};
}

void Anthill::addTunnel(const std::string& a, const std::string& b) {
    auto& va = adjacency[a];
    if (std::find(va.begin(), va.end(), b) == va.end()) va.push_back(b);
    auto& vb = adjacency[b];
    if (std::find(vb.begin(), vb.end(), a) == vb.end()) vb.push_back(a);
}

// ─── Sélection des K meilleurs chemins ───────────
// Stratégie : garder les chemins dont la longueur
// ne dépasse pas longueur_min + marge.
// Cela évite d'utiliser des chemins très longs qui
// ralentissent la simulation (surtout sur fourmiliere_3D).

std::vector<std::vector<std::string>> Anthill::selectBestPaths(
    std::vector<std::vector<std::string>> paths) const
{
    if (paths.empty()) return paths;

    // Longueur du chemin le plus court
    size_t longueurMin = paths[0].size();

    // Marge : on accepte les chemins jusqu'à 2 tunnels de plus que le plus court
    const size_t MARGE = 2;

    std::vector<std::vector<std::string>> selected;
    for (auto& p : paths) {
        if (p.size() <= longueurMin + MARGE)
            selected.push_back(p);
    }

    // Limiter à 8 chemins maximum pour éviter les assignations trop fragmentées
    if (selected.size() > 8)
        selected.resize(8);

    return selected;
}

// ─── Simulation commune ───────────────────────────
// Appelée par simulate(), simulateDijkstra(), simulateDFS()
// Répartit les fourmis sur les chemins et simule étape par étape.

void Anthill::runSimulation(const std::vector<std::vector<std::string>>& paths,
                             const std::string& algoName) {
    steps.clear();

    std::vector<Ant> ants;
    for (int i = 1; i <= nbAnts; i++)
        ants.emplace_back("f" + std::to_string(i), vestibule);

    // Assignation gloutonne : score = nb_fourmis_sur_ce_chemin × longueur
    std::vector<int> antPathIdx(nbAnts, 0);
    std::vector<int> antStepIdx(nbAnts, 0);
    std::vector<int> pathLoad(paths.size(), 0);

    for (int i = 0; i < nbAnts; i++) {
        int best = 0, bestScore = INT_MAX;
        for (int p = 0; p < (int)paths.size(); p++) {
            int score = pathLoad[p] * (int)paths[p].size();
            if (score < bestScore) { bestScore = score; best = p; }
        }
        antPathIdx[i] = best;
        pathLoad[best]++;
    }

    for (int s = 0; s < 1000000; s++) {
        bool allDone = true;
        for (auto& ant : ants) if (!ant.inDortoir) { allDone = false; break; }
        if (allDone) break;

        std::vector<Move> stepMoves;
        std::map<std::string, int> nextOcc;

        for (int i = 0; i < nbAnts; i++) {
            Ant& ant = ants[i];
            if (ant.inDortoir) continue;

            const auto& path = paths[antPathIdx[i]];
            int pos = antStepIdx[i];
            if (pos + 1 >= (int)path.size()) continue;
            std::string next = path[pos + 1];

            bool canMove = false;
            if (next == dortoir) {
                canMove = true;
            } else {
                int cap  = rooms.at(next).capacity;
                int used = nextOcc.count(next) ? nextOcc[next] : 0;
                canMove  = (used < cap);
            }

            if (canMove) {
                stepMoves.push_back({ant.id, ant.currentRoom, next});
                if (next != dortoir) nextOcc[next]++;
                ant.currentRoom = next;
                antStepIdx[i]++;
                if (next == dortoir) ant.inDortoir = true;
            } else {
                if (ant.currentRoom != vestibule)
                    nextOcc[ant.currentRoom]++;
            }
        }

        if (!stepMoves.empty()) steps.push_back(stepMoves);
    }

    // Affichage des étapes
    for (int i = 0; i < (int)steps.size(); i++) {
        std::cout << "+++ E" << (i+1) << " +++\n";
        for (auto& m : steps[i])
            std::cout << m.antId << " - " << m.from << " - " << m.to << "\n";
    }
    std::cout << "=> " << steps.size() << " etape(s) ["
              << algoName << "] pour " << nbAnts << " fourmi(s)\n\n";
}

// ─── Affichage ────────────────────────────────────

void Anthill::printGraph() const {
    std::cout << "=== Graphe (" << nbAnts << " fourmi(s)) ===\n";
    for (auto& [name, room] : rooms) {
        std::cout << "  " << name;
        if (room.isVestibule)         std::cout << " (vestibule)";
        else if (room.isDortoir)      std::cout << " (dortoir)";
        else if (room.capacity == INT_MAX) std::cout << " {inf}";
        else std::cout << " {" << room.capacity << "}";
        std::cout << "\n";
    }
    std::set<std::string> seen;
    for (auto& [from, nbs] : adjacency)
        for (auto& to : nbs) {
            std::string k = (from < to) ? from+"-"+to : to+"-"+from;
            if (!seen.count(k)) {
                std::cout << "  tunnel: " << from << " - " << to << "\n";
                seen.insert(k);
            }
        }
    std::cout << "\n";
}

void Anthill::printSteps() const {
    for (int i = 0; i < (int)steps.size(); i++) {
        std::cout << "+++ E" << (i+1) << " +++\n";
        for (auto& m : steps[i])
            std::cout << m.antId << " - " << m.from << " - " << m.to << "\n";
    }
    std::cout << "=> " << steps.size() << " etape(s)\n\n";
}

// ─── Parser ───────────────────────────────────────
// Format : f=N ou F=N, puis salles (une par ligne),
// puis tunnels (ligne avec " - "), capacité en { X }

static std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    return (a == std::string::npos) ? "" : s.substr(a, b - a + 1);
}

Anthill parseFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Impossible d'ouvrir : " + filename);

    std::string line;
    std::getline(file, line);
    line = trim(line);

    // Accepter f= et F=
    if (line.size() < 3 || (line.substr(0,2) != "f=" && line.substr(0,2) != "F="))
        throw std::runtime_error("Format invalide : doit commencer par f=N ou F=N");

    int nbAnts = std::stoi(line.substr(2));
    Anthill anthill(nbAnts);

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        if (line.find(" - ") != std::string::npos) {
            // Tunnel
            auto dash = line.find(" - ");
            anthill.addTunnel(trim(line.substr(0, dash)),
                              trim(line.substr(dash + 3)));
        } else {
            // Salle (avec capacité optionnelle en { X })
            int cap = 1;
            std::string name = line;
            auto lb = line.find('{');
            if (lb != std::string::npos) {
                auto rb = line.find('}', lb);
                cap  = std::stoi(trim(line.substr(lb + 1, rb - lb - 1)));
                name = trim(line.substr(0, lb));
            }
            if (name != "Sv" && name != "Sd")
                anthill.addRoom(Room(name, cap, false, false));
        }
    }
    return anthill;
}