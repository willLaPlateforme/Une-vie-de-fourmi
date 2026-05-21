// DIJKSTRA
// Algorithme qui permet de trouver le chemin le plus court
// entre le vestibule (départ) et le dortoir (arrivée).
//
// Contrairement au BFS/DFS, il tient compte de la distance
// et choisit toujours le chemin le plus "court" en priorité.

std::vector<std::string> Anthill::findShortestPathDijkstra() const
{
    // dist = distance minimale connue pour chaque salle
    // Exemple : combien de tunnels pour atteindre chaque salle
    std::map<std::string, int> dist;

    // pred = permet de reconstruire le chemin final
    // pred[A] = B signifie : on est arrivé à A depuis B
    std::map<std::string, std::string> pred;

    // On initialise toutes les distances à "infini"
    // (INT_MAX = très grand nombre = pas encore atteint)
    for (auto& [name, _] : adjacency)
        dist[name] = INT_MAX;

    // La distance du point de départ (vestibule) est 0
    dist[vestibule] = 0;

    // Priority queue = file de priorité
    // Elle permet de toujours traiter la salle la plus proche en premier
    //
    // format : (distance, salle)
    std::priority_queue<
        std::pair<int, std::string>,
        std::vector<std::pair<int, std::string>>,
        std::greater<>
    > pq;

    // On commence avec le vestibule
    pq.push({0, vestibule});

    // Tant qu'il reste des salles à traiter
    while (!pq.empty())
    {
        // On récupère la salle avec la plus petite distance
        auto [cout_actuel, salle] = pq.top();
        pq.pop();

        // Si on a déjà trouvé mieux avant, on ignore cette version
        if (cout_actuel > dist[salle])
            continue;

        // Si on arrive au dortoir, on peut arrêter
        if (salle == dortoir)
            break;

        // On récupère les voisins de la salle actuelle
        auto it = adjacency.find(salle);

        // Si aucune connexion, on passe
        if (it == adjacency.end())
            continue;

        // On explore tous les voisins
        for (const std::string& voisin : it->second)
        {
            // On calcule la nouvelle distance
            // ici chaque déplacement coûte 1
            int nouveau = dist[salle] + 1;

            // Si on trouve un chemin plus court
            if (nouveau < dist[voisin])
            {
                // On met à jour la meilleure distance trouvée
                dist[voisin] = nouveau;

                // On enregistre d'où on vient
                pred[voisin] = salle;

                // On ajoute la salle dans la file de priorité
                pq.push({nouveau, voisin});
            }
        }
    }

    // Si le dortoir n'est jamais atteint
    if (dist[dortoir] == INT_MAX)
        throw std::runtime_error("Dijkstra : aucun chemin trouvé !");

    // -------------------------------
    // Reconstruction du chemin final
    // -------------------------------

    std::vector<std::string> chemin;

    // On part du dortoir
    std::string courant = dortoir;

    // On remonte les prédécesseurs jusqu'au vestibule
    while (courant != vestibule)
    {
        chemin.push_back(courant);
        courant = pred[courant];
    }

    // On ajoute le point de départ
    chemin.push_back(vestibule);

    // On inverse car on a construit le chemin à l'envers
    std::reverse(chemin.begin(), chemin.end());

    // On retourne le chemin le plus court trouvé
    return chemin;
}


// -------------------------------------------------------------------


// Fonction qui lance le Dijkstra et affiche le résultat

void Anthill::simulateDijkstra()
{
    // On récupère le chemin le plus court
    auto chemin = findShortestPathDijkstra();

    // Affichage du résultat
    std::cout << "[Dijkstra] Chemin optimal : ";

    for (size_t i = 0; i < chemin.size(); i++)
    {
        std::cout << chemin[i];

        if (i + 1 < chemin.size())
            std::cout << " -> ";
    }

    std::cout << " (" << chemin.size() - 1 << " tunnel(s))\n";

    // Lancement de la simulation avec ce chemin
    runSimulation({chemin}, "Dijkstra");
}