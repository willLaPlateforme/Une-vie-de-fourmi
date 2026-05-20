#include "ants.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <map>
#include <algorithm>

// ─── Couleurs ANSI ───────────────────────────────
#define RESET    "\033[0m"
#define BOLD     "\033[1m"
#define DIM      "\033[2m"
#define RED      "\033[31m"
#define GREEN    "\033[32m"
#define YELLOW   "\033[33m"
#define BLUE     "\033[34m"
#define MAGENTA  "\033[35m"
#define CYAN     "\033[36m"
#define CLEAR    "\033[2J\033[H"

void pause(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// ─── Menu principal ───────────────────────────────
void afficherMenu() {
    std::cout << CLEAR;
    std::cout << BOLD << CYAN;
    std::cout << "  ╔══════════════════════════════════════════╗\n";
    std::cout << "  ║         🐜  Une vie de fourmi  🐜        ║\n";
    std::cout << "  ╚══════════════════════════════════════════╝\n";
    std::cout << RESET << "\n";
    std::cout << BOLD << "  Choisissez un algorithme :\n\n" << RESET;
    std::cout << "  " << CYAN   << "[1]" << RESET << " BFS       — tous les chemins, fourmis en parallèle\n";
    std::cout << "  " << GREEN  << "[2]" << RESET << " Dijkstra  — chemin le plus court, file indienne\n";
    std::cout << "  " << YELLOW << "[3]" << RESET << " DFS       — exploration en profondeur + backtracking\n";
    std::cout << "  " << RED    << "[0]" << RESET << " Quitter\n\n";
    std::cout << "  Votre choix : ";
}

// ─── Menu fourmilière ─────────────────────────────
int choisirFourmiliere(const std::vector<std::string>& fichiers) {
    std::cout << CLEAR;
    std::cout << BOLD << CYAN;
    std::cout << "  ╔══════════════════════════════════════════╗\n";
    std::cout << "  ║         Choisir une fourmilière          ║\n";
    std::cout << "  ╚══════════════════════════════════════════╝\n";
    std::cout << RESET << "\n";
    for (int i = 0; i < (int)fichiers.size(); i++)
        std::cout << "  " << CYAN << "[" << i+1 << "]" << RESET
                  << " " << fichiers[i] << "\n";
    std::cout << "  " << CYAN << "[" << fichiers.size()+1 << "]" << RESET
              << " Toutes les fourmilières\n";
    std::cout << "\n  Votre choix : ";
    int c; std::cin >> c; return c;
}

// ─── Carte visuelle ───────────────────────────────
void afficherCarte(
    const std::map<std::string, std::vector<std::string>>& adjacency,
    const std::map<std::string, std::string>& antPositions,
    const std::string& vestibule,
    const std::string& dortoir,
    int etape, int totalFourmis, int dansLeDortoir)
{
    std::cout << CLEAR;
    std::cout << BOLD << CYAN;
    std::cout << "  ╔══════════════════════════════════════════╗\n";
    std::cout << "  ║  Etape " << etape
              << "   |  Fourmis au dortoir : "
              << GREEN << dansLeDortoir << "/" << totalFourmis
              << CYAN  << "          ║\n";
    std::cout << "  ╚══════════════════════════════════════════╝\n" << RESET << "\n";

    std::vector<std::string> salles;
    std::set<std::string> vues;
    for (auto& [s, _] : adjacency)
        if (!vues.count(s)) { salles.push_back(s); vues.insert(s); }
    std::sort(salles.begin(), salles.end());

    for (auto& salle : salles) {
        std::vector<std::string> fourmisIci;
        for (auto& [ant, pos] : antPositions)
            if (pos == salle) fourmisIci.push_back(ant);

        std::string couleur = "\033[37m";
        std::string label   = salle;
        if (salle == vestibule) { couleur = BLUE;  label = salle + " (vestibule)"; }
        if (salle == dortoir)   { couleur = GREEN; label = salle + " (dortoir)  "; }

        std::cout << "  " << couleur << BOLD << "[ " << label << " ]" << RESET;
        if (!fourmisIci.empty()) {
            std::cout << "  " << YELLOW;
            // Afficher max 8 fourmis pour ne pas déborder
            int shown = 0;
            for (auto& f : fourmisIci) {
                if (shown >= 8) { std::cout << "+" << (fourmisIci.size()-8) << " autres"; break; }
                std::cout << "🐜" << f << " "; shown++;
            }
            std::cout << RESET;
        }
        std::cout << "\n";

        auto it = adjacency.find(salle);
        if (it != adjacency.end()) {
            for (auto& voisin : it->second) {
                if (voisin > salle || voisin == dortoir)
                    std::cout << DIM << "      └─▶ " << voisin << "\n" << RESET;
            }
        }
    }
    std::cout << "\n";
}

// ─── Affichage étape ─────────────────────────────
void afficherEtape(const std::vector<Move>& moves, int num) {
    std::cout << BOLD << MAGENTA << "  +++ E" << num << " +++\n" << RESET;
    for (auto& m : moves)
        std::cout << "  " << YELLOW << m.antId << RESET
                  << " : " << CYAN << m.from << RESET
                  << " ──▶ " << GREEN << m.to << RESET << "\n";
    std::cout << "\n";
}

// ─── Simulation avec visuel ───────────────────────
void simulerAvecVisuel(Anthill& anthill, int algo) {
    if      (algo == 1) anthill.simulate();
    else if (algo == 2) anthill.simulateDijkstra();
    else                anthill.simulateDFS();

    const auto& steps    = anthill.getSteps();
    int          nbAnts  = anthill.getNbAnts();
    const auto&  adj     = anthill.getAdjacency();
    const auto&  vest    = anthill.getVestibule();
    const auto&  dort    = anthill.getDortoir();

    std::map<std::string, std::string> positions;
    for (int i = 1; i <= nbAnts; i++)
        positions["f" + std::to_string(i)] = vest;

    int dansLeDortoir = 0;

    for (int i = 0; i < (int)steps.size(); i++) {
        for (auto& m : steps[i]) {
            positions[m.antId] = m.to;
            if (m.to == dort) dansLeDortoir++;
        }
        afficherCarte(adj, positions, vest, dort, i+1, nbAnts, dansLeDortoir);
        afficherEtape(steps[i], i+1);
        std::cout << DIM << "  [ Entrée = étape suivante | q+Entrée = quitter ]" << RESET << " ";
        std::string inp; std::getline(std::cin, inp);
        if (inp == "q") break;
    }

    // Résumé final
    std::cout << CLEAR << BOLD << GREEN;
    std::cout << "  ╔══════════════════════════════════════════╗\n";
    std::cout << "  ║          ✅  Simulation terminée !       ║\n";
    std::cout << "  ╚══════════════════════════════════════════╝\n" << RESET << "\n";
    std::cout << "  " << BOLD << nbAnts << RESET << " fourmi(s) → dortoir en "
              << BOLD << YELLOW << steps.size() << " étape(s)" << RESET << "\n\n";
    std::cout << DIM << "  [ Entrée pour continuer... ]" << RESET;
    std::string tmp; std::getline(std::cin, tmp);
}

// ─── Programme principal ─────────────────────────
int main() {
    std::vector<std::string> fichiers = {
        "fourmiliere_zero.txt",
        "fourmiliere_un.txt",
        "fourmiliere_deux.txt",
        "fourmiliere_trois.txt",
        "fourmiliere_quatre.txt",
        "fourmiliere_cinq.txt",
        "fourmiliere_3D.txt",
        "La_hormiguera_de_la_muerte.txt",
        "salle_d_at-ant.txt"
    };

    std::vector<std::string> nomAlgo    = {"", "BFS", "Dijkstra", "DFS"};
    std::vector<std::string> coulAlgo   = {"", CYAN, GREEN, YELLOW};

    // Vider le buffer avant la boucle
    std::cin.ignore();

    while (true) {
        afficherMenu();
        int algo; std::cin >> algo;
        std::cin.ignore();
        if (algo == 0) { std::cout << "\n  Au revoir ! 🐜\n\n"; break; }
        if (algo < 1 || algo > 3) continue;

        int choixF = choisirFourmiliere(fichiers);
        std::cin.ignore();
        if (choixF < 1 || choixF > (int)fichiers.size() + 1) continue;

        std::vector<std::string> aTraiter;
        if (choixF == (int)fichiers.size() + 1) aTraiter = fichiers;
        else aTraiter = {fichiers[choixF - 1]};

        for (auto& f : aTraiter) {
            std::cout << CLEAR << BOLD << coulAlgo[algo]
                      << "\n  Algorithme : " << nomAlgo[algo]
                      << "  |  Fichier : " << f << "\n\n" << RESET;
            pause(400);
            try {
                Anthill anthill = parseFile(f);
                simulerAvecVisuel(anthill, algo);
            } catch (const std::exception& e) {
                std::cerr << RED << "  Erreur : " << e.what() << RESET << "\n";
                std::cout << DIM << "  [ Entrée pour continuer... ]" << RESET;
                std::string tmp; std::getline(std::cin, tmp);
            }
        }
    }
    return 0;
}