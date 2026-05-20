std::vector<std::string> Anthill::findShortestPathDijkstra() const {
    std::map<std::string, int>         dist;
    std::map<std::string, std::string> pred;

    for (auto& [name, _] : adjacency) dist[name] = INT_MAX;
    dist[vestibule] = 0;

    // (coût, salle) — on traite le coût le plus bas en premier
    std::priority_queue<
        std::pair<int,std::string>,
        std::vector<std::pair<int,std::string>>,
        std::greater<>> pq;
    pq.push({0, vestibule});

    while (!pq.empty()) {
        auto [cout_actuel, salle] = pq.top(); pq.pop();
        if (cout_actuel > dist[salle]) continue;
        if (salle == dortoir) break;

        auto it = adjacency.find(salle);
        if (it == adjacency.end()) continue;
        for (const std::string& voisin : it->second) {
            int nouveau = dist[salle] + 1;
            if (nouveau < dist[voisin]) {
                dist[voisin] = nouveau;
                pred[voisin] = salle;
                pq.push({nouveau, voisin});
            }
        }
    }

    if (dist[dortoir] == INT_MAX)
        throw std::runtime_error("Dijkstra : aucun chemin trouvé !");

    // Reconstruction en remontant les prédécesseurs
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
    auto chemin = findShortestPathDijkstra();
    std::cout << "[Dijkstra] Chemin optimal : ";
    for (size_t i = 0; i < chemin.size(); i++) {
        std::cout << chemin[i];
        if (i + 1 < chemin.size()) std::cout << " -> ";
    }
    std::cout << " (" << chemin.size() - 1 << " tunnel(s))\n";
    runSimulation({chemin}, "Dijkstra");
}
