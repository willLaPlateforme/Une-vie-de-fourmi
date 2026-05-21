#ifndef ANTS_HPP
#define ANTS_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include <climits>

struct Room{
    std::string name;
    int capacity;
    bool isVestibule;
    bool isDortoir;

    Room() : name(""), capacity(1), isVestibule(false), isDortoir(false) {}
    Room(const std::string& n, int cap = 1, bool sv =false, bool sd = false)
    : name(n), capacity(cap), isVestibule(sv), isDortoir(sd) {}
};

struct Ant {

    std::string id;
    std::string currentRoom;

    bool inDortoir;
    Ant(const std::string& id, const std::string&start)
    : id(id), currentRoom(start), inDortoir(false){}
};


struct Move {
    std::string antId;
    std::string from;
    std::string to;
};

class Anthill{

public:
    Anthill(int nbAnts);
    void addRoom(const Room& room);
    void addTunnel(const std::string& a, const std::string& b);

    //Algorithme 1 : BFS ()
    void simulate();
    //Algorithme 2 : Dijkstra
    void simulateDijkstra();
    //Algorithme 3 : DFS (deepfirst)
    void simulateDFS();

    void printGraph() const;
    void printSteps() const;

    //Getters pour le visuel

    const std::vector<std::vector<Move>>& getSteps()   const { return steps; }
    int                                   getNbAnts()   const { return nbAnts; }
    const std::map<std::string, std::vector<std::string>>& getAdjacency() const { return adjacency; }
    const std::string&                    getVestibule() const { return vestibule; }
    const std::string&                    getDortoir()   const { return dortoir; }

private:

    int                                              nbAnts;
    std::map<std::string, Room>                      rooms;
    std::map<std::string, std::vector< std::string>> adjacency;
    std::string                                      dortoir;
    std::vector<std::vector<Move>>                   steps;

    std::vector<std::vector<std::std::string>> findAllPathsBFS() const;
    std::vector<std::string>                   findShortestPathDijkstra() const;
    std::vector<std::vector<string>>           findAllPathsDFS() const;
    void dfsHelper(const std:: string& current,
                   std::vector<std::vector<std::string>>& path,
                   std::vector<std::vector<std::strings>>& result) const;
    
    void runSimulation(const std::vector<std::vector<std::string>>& paths,
                       const std::string& algoName);
};


Anthill parseFile (const std::string& filename);

#endif

// #ifndef ANTS_HPP
// #define ANTS_HPP

// #include <string>
// #include <vector>
// #include <map>
// #include <set>
// #include <climits>

// using namespace std;

// struct Room {

//     string name;
//     int capacity;
//     bool isVestibule;
//     bool isDortoir;

//     Room() : name(""), capacity(1), isVestibule(false), isDortoir(false) {}

//     Room(const string& n, int cap = 1, bool sv = false, bool sd = false)
//     : name(n), capacity(cap), isVestibule(sv), isDortoir(sd) {}
// };

// struct Ant {

//     string id;
//     string currentRoom;
//     bool inDortoir;

//     Ant(const string& id, const string& start)
//     : id(id), currentRoom(start), inDortoir(false) {}
// };

// struct Move {

//     string antId;
//     string from;
//     string to;
// };

// class Anthill {

// public:

//     Anthill(int nbAnts);

//     void addRoom(const Room& room);

//     void addTunnel(const string& a, const string& b);

//     // Algorithme 1 : BFS
//     void simulate();

//     // Algorithme 2 : Dijkstra
//     void simulateDijkstra();

//     // Algorithme 3 : DFS
//     void simulateDFS();

//     void printGraph() const;

//     void printSteps() const;

//     // Getters pour le visuel

//     const vector<vector<Move>>& getSteps() const {
//         return steps;
//     }

//     int getNbAnts() const {
//         return nbAnts;
//     }

//     const map<string, vector<string>>& getAdjacency() const {
//         return adjacency;
//     }

//     const string& getVestibule() const {
//         return vestibule;
//     }

//     const string& getDortoir() const {
//         return dortoir;
//     }

// private:

//     int nbAnts;

//     map<string, Room> rooms;

//     map<string, vector<string>> adjacency;

//     string vestibule;

//     string dortoir;

//     vector<vector<Move>> steps;

//     vector<vector<string>> findAllPathsBFS() const;

//     vector<string> findShortestPathDijkstra() const;

//     vector<vector<string>> findAllPathsDFS() const;

//     void dfsHelper(const string& current,
//                    vector<string>& path,
//                    vector<vector<string>>& result) const;

//     void runSimulation(const vector<vector<string>>& paths,
//                        const string& algoName);
// };

// Anthill parseFile(const string& filename);

// #endif