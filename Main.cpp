#include "ants.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <map>
#include <algorithm>
#include <climits>

// ─── Couleurs ANSI ───────────────────────────────
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BG_BLUE "\033[44m"
#define BG_GREEN "\033[42m"
#define BG_RED  "\033[41m"
#define CLEAR_SCREEN "\033[2J\033[H"

// ─── Pause entre étapes ───────────────────────────
void pause(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// ─── Affiche le menu principal ────────────────────
void afficherMenu() {
    std::cout << CLEAR_SCREEN;
    std::cout << BOLD << CYAN;
    std::cout << "  ╔══════════════════════════════════════════╗\n";
    std::cout << "  ║         🐜  Une vie de fourmi  🐜        ║\n";
    std::cout << "  ╚══════════════════════════════════════════╝\n";
    std::cout << RESET << "\n";

    std::cout << BOLD << "  Choisissez un algorithme :\n\n" << RESET;
    std::cout << "  " << CYAN  << "[1]" << RESET << " BFS       — tous les chemins, fourmis en parallèle\n";
    std::cout << "  " << GREEN << "[2]" << RESET << " Dijkstra  — chemin le plus court, file indienne\n";
    std::cout << "  " << YELLOW<< "[3]" << RESET << " DFS       — exploration en profondeur + backtracking\n";
    std::cout << "  " << RED   << "[0]" << RESET << " Quitter\n\n";
    std::cout << "  Votre choix : ";
}

// ─── Affiche le menu de sélection de fourmilière ─
int choisirFourmiliere(const std::vector<std::string>& fichiers) {
    std::cout << CLEAR_SCREEN;
    std::cout << BOLD << CYAN;
    std::cout << "  ╔══════════════════════════════════════════╗\n";
    std::cout << "  ║         Choisir une fourmilière          ║\n";
    std::cout << "  ╚══════════════════════════════════════════╝\n";
    std::cout << RESET << "\n";

    for (int i = 0; i < (int)fichiers.size(); i++) {
        std::cout << "  " << CYAN << "[" << i+1 << "]" << RESET
                  << " " << fichiers[i] << "\n";
    }
    std::cout << "  " << CYAN << "[" << fichiers.size()+1 << "]" << RESET
              << " Toutes les fourmilières\n";
    std::cout << "\n  Votre choix : ";

    int choix;
    std::cin >> choix;
    return choix;
}

// ─── Affiche la carte de la fourmilière ──────────
void afficherCarte(
    const std::map<std::string, std::vector<std::string>>& adjacency,
    const std::map<std::string, std::string>& antPositions,
    const std::string& vestibule,
    const std::string& dortoir,
    int etape, int totalFourmis, int dansLeDortoir)
{
    std::cout << CLEAR_SCREEN;
    std::cout << BOLD << CYAN;
    std::cout << "  ╔══════════════════════════════════════════╗\n";
    std::cout << "  ║  Etape " << etape;
    std::cout << "   |  Fourmis au dortoir : "
              << GREEN << dansLeDortoir << "/" << totalFourmis
              << CYAN << "          ║\n";
    std::cout << "  ╚══════════════════════════════════════════╝\n";
    std::cout << RESET << "\n";

    // Collecter les salles uniques
    std::vector<std::string> salles;
    std::set<std::string> vues;
    for (auto& [s, _] : adjacency) {
        if (!vues.count(s)) { salles.push_back(s); vues.insert(s); }
    }
    std::sort(salles.begin(), salles.end());

    // Afficher chaque salle
    for (auto& salle : salles) {
        // Fourmis présentes dans cette salle
        std::vector<std::string> fourmisIci;
        for (auto& [ant, pos] : antPositions)
            if (pos == salle) fourmisIci.push_back(ant);

        // Couleur selon le type de salle
        std::string couleur = WHITE;
        std::string label   = salle;
        if (salle == vestibule) { couleur = BLUE;  label = "Sv (vestibule)"; }
        if (salle == dortoir)   { couleur = GREEN; label = "Sd (dortoir)  "; }

        std::cout << "  " << couleur << BOLD << "[ " << label << " ]" << RESET;

        if (!fourmisIci.empty()) {
            std::cout << "  " << YELLOW;
            for (auto& f : fourmisIci) std::cout << "🐜 " << f << " ";
            std::cout << RESET;
        }
        std::cout << "\n";

        // Afficher les tunnels sortants
        auto it = adjacency.find(salle);
        if (it != adjacency.end()) {
            for (auto& voisin : it->second) {
                if (voisin > salle || voisin == dortoir) {
                    std::cout << DIM << "      |-- tunnel --> " << voisin << "\n" << RESET;
                }
            }
        }
    }

    std::cout << "\n";
}

// ─── Affiche un résumé d'une étape ───────────────
void afficherEtape(const std::vector<Move>& moves, int numEtape) {
    std::cout << BOLD << MAGENTA
              << "  +++ E" << numEtape << " +++\n" << RESET;
    for (auto& m : moves) {
        std::cout << "  " << YELLOW << m.antId << RESET
                  << " : " << CYAN << m.from
                  << RESET << " --> " << GREEN << m.to << RESET << "\n";
    }
    std::cout << "\n";
}

// ─── Simulation avec visuel ───────────────────────
void simulerAvecVisuel(Anthill& anthill, int algo, int nbAnts,
                        const std::map<std::string,
                        std::vector<std::string>>& adjacency,
                        const std::string& vestibule,
                        const std::string& dortoir)
{
    // Lancer la simulation pour récupérer les étapes
    if      (algo == 1) anthill.simulate();
    else if (algo == 2) anthill.simulateDijkstra();
    else                anthill.simulateDFS();

    const auto& steps = anthill.getSteps();

    // Reconstruire les positions fourmi par fourmi
    std::map<std::string, std::string> positions;
    for (int i = 1; i <= nbAnts; i++)
        positions["f" + std::to_string(i)] = vestibule;

    int dansLeDortoir = 0;

    // Afficher étape par étape avec animation
    for (int i = 0; i < (int)steps.size(); i++) {
        // Mettre à jour les positions
        for (auto& m : steps[i]) {
            positions[m.antId] = m.to;
            if (m.to == dortoir) dansLeDortoir++;
        }

        afficherCarte(adjacency, positions, vestibule, dortoir,
                      i + 1, nbAnts, dansLeDortoir);
        afficherEtape(steps[i], i + 1);

        std::cout << DIM << "  [ Appuyez sur Entrée pour l'étape suivante... ]" << RESET;
        std::cin.ignore(); std::cin.get();
    }

    // Résultat final
    std::cout << CLEAR_SCREEN;
    std::cout << BOLD << GREEN;
    std::cout << "  ╔══════════════════════════════════════════╗\n";
    std::cout << "  ║          ✅  Simulation terminée !       ║\n";
    std::cout << "  ╚══════════════════════════════════════════╝\n";
    std::cout << RESET << "\n";
    std::cout << "  " << nbAnts << " fourmi(s) ont rejoint le dortoir en "
              << BOLD << YELLOW << steps.size() << " étape(s)" << RESET << ".\n\n";

    std::cout << DIM << "  [ Appuyez sur Entrée pour continuer... ]" << RESET;
    std::cin.get();
}

// ─── Point d'entrée ───────────────────────────────
int main() {
    std::vector<std::string> fichiers = {
        "fourmiliere_zero.txt",
        "fourmiliere_un.txt",
        "fourmiliere_deux.txt",
        "fourmiliere_trois.txt",
        "fourmiliere_quatre.txt",
        "fourmiliere_cinq.txt"
    };

    std::vector<std::string> nomAlgo = {"", "BFS", "Dijkstra", "DFS"};
    std::vector<std::string> couleurAlgo = {"", CYAN, GREEN, YELLOW};

    while (true) {
        afficherMenu();
        int algo;
        std::cin >> algo;
        if (algo == 0) {
            std::cout << "\n  Au revoir ! 🐜\n\n";
            break;
        }
        if (algo < 1 || algo > 3) continue;

        int choixF = choisirFourmiliere(fichiers);
        if (choixF < 1 || choixF > (int)fichiers.size() + 1) continue;

        std::vector<std::string> aTraiter;
        if (choixF == (int)fichiers.size() + 1)
            aTraiter = fichiers;
        else
            aTraiter = {fichiers[choixF - 1]};

        for (auto& f : aTraiter) {
            try {
                std::cout << CLEAR_SCREEN;
                std::cout << BOLD << couleurAlgo[algo]
                          << "  Algorithme : " << nomAlgo[algo]
                          << "  |  Fichier : " << f << "\n\n" << RESET;
                pause(500);

                Anthill anthill = parseFile(f);

                // Récupérer infos pour le visuel
                simulerAvecVisuel(anthill, algo,
                                  anthill.getNbAnts(),
                                  anthill.getAdjacency(),
                                  anthill.getVestibule(),
                                  anthill.getDortoir());

            } catch (const std::exception& e) {
                std::cerr << RED << "  Erreur : " << e.what() << RESET << "\n";
                std::cin.ignore(); std::cin.get();
            }
        }
    }
    return 0;
}