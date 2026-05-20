#ifndef ANTS_HPP
#define ANTS_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include <climits>

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

class Anthill {
public:
    Anthill(int nbAnts);
    void addRoom(const Room& room);
    void addTunnel(const std::string& a, const std::string& b);

    // Algorithme 1 : BFS
    void simulate();
    // Algorithme 2 : Dijkstra
    void simulateDijkstra();
    // Algorithme 3 : DFS
    void simulateDFS();

    void printGraph() const;
    void printSteps() const;

    // Getters pour le visuel
    const std::vector<std::vector<Move>>& getSteps()     const { return steps; }
    int                                   getNbAnts()    const { return nbAnts; }
    const std::map<std::string, std::vector<std::string>>& getAdjacency() const { return adjacency; }
    const std::string&                    getVestibule() const { return vestibule; }
    const std::string&                    getDortoir()   const { return dortoir; }

private:
    int                                              nbAnts;
    std::map<std::string, Room>                      rooms;
    std::map<std::string, std::vector<std::string>>  adjacency;
    std::string                                      vestibule;
    std::string                                      dortoir;
    std::vector<std::vector<Move>>                   steps;

    std::vector<std::vector<std::string>> findAllPathsBFS() const;
    std::vector<std::string>              findShortestPathDijkstra() const;
    std::vector<std::vector<std::string>> findAllPathsDFS() const;
    void dfsHelper(const std::string& current,
                   std::vector<std::string>& path,
                   std::vector<std::vector<std::string>>& result) const;
    void runSimulation(const std::vector<std::vector<std::string>>& paths,
                       const std::string& algoName);
};

Anthill parseFile(const std::string& filename);

#endif