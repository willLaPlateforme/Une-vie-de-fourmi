#ifndef ANTS_HPP
#define ANTS_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include <climits>

// ─── Structures de base ───────────────────────────

struct Room {
    std::string name;
    int         capacity;
    bool        isVestibule;
    bool        isDortoir;

    Room() : name(""), capacity(1), isVestibule(false), isDortoir(false) {}
    Room(const std::string& n, int cap = 1, bool sv = false, bool sd = false)
        : name(n), capacity(cap), isVestibule(sv), isDortoir(sd) {}
};

struct Ant {
    std::string id;
    std::string currentRoom;
    bool        inDortoir;
    Ant(const std::string& id, const std::string& start)
        : id(id), currentRoom(start), inDortoir(false) {}
};

struct Move {
    std::string antId;
    std::string from;
    std::string to;
};

// ─── Classe Anthill ───────────────────────────────

class Anthill {
public:
    Anthill(int nbAnts);
    void addRoom(const Room& room);
    void addTunnel(const std::string& a, const std::string& b);

    void simulate();          // BFS
    void simulateDijkstra();  // Dijkstra
    void simulateDFS();       // DFS

    void printGraph() const;
    void printSteps() const;

    // Getters pour le visuel dans main.cpp
    const std::vector<std::vector<Move>>& getSteps()      const { return steps; }
    int                                   getNbAnts()     const { return nbAnts; }
    const std::map<std::string, std::vector<std::string>>& getAdjacency() const { return adjacency; }
    const std::string&                    getVestibule()  const { return vestibule; }
    const std::string&                    getDortoir()    const { return dortoir; }
    const std::map<std::string, Room>&    getRooms()      const { return rooms; }

private:
    int                                              nbAnts;
    std::map<std::string, Room>                      rooms;
    std::map<std::string, std::vector<std::string>>  adjacency;
    std::string                                      vestibule;
    std::string                                      dortoir;
    std::vector<std::vector<Move>>                   steps;

    // Méthodes privées de recherche de chemins
    std::vector<std::vector<std::string>> findAllPathsBFS()  const;
    std::vector<std::string>              findPathDijkstra() const;
    std::vector<std::vector<std::string>> findAllPathsDFS()  const;
    void dfsHelper(const std::string& current,
                   std::vector<std::string>& path,
                   std::vector<std::vector<std::string>>& result) const;

    // Sélectionne les K meilleurs chemins (les plus courts) parmi une liste
    std::vector<std::vector<std::string>> selectBestPaths(
        std::vector<std::vector<std::string>> paths) const;

    // Simulation commune aux 3 algorithmes
    void runSimulation(const std::vector<std::vector<std::string>>& paths,
                       const std::string& algoName);
};

// ─── Parser ───────────────────────────────────────
Anthill parseFile(const std::string& filename);

#endif