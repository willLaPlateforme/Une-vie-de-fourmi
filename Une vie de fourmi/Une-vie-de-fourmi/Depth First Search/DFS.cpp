// DFS (Depth First Search)
// Cette fonction trouve TOUS les chemins possibles
// entre le vestibule (entrée) et le dortoir (sortie).
//
// Elle ne fait pas tout toute seule :
// elle utilise une fonction récursive (dfsHelper)
// pour explorer le graphe pas à pas.

std::vector<std::vector<std::string>> Anthill::findAllPathsDFS() const
{
    // Liste finale de tous les chemins trouvés
    // Chaque chemin est une suite de salles (ex : A -> B -> C -> D)
    std::vector<std::vector<std::string>> result;

    // Chemin en cours de construction
    // Il représente le trajet actuel d’une fourmi
    std::vector<std::string> path;

    // On démarre la recherche depuis le vestibule
    dfsHelper(vestibule, path, result);

    // On retourne tous les chemins trouvés
    return result;
}


// -------------------------------------------------------------------


// Fonction récursive DFS
// Elle explore toutes les salles connectées
// jusqu’à atteindre le dortoir.

void Anthill::dfsHelper(const std::string& current,
                        std::vector<std::string>& path,
                        std::vector<std::vector<std::string>>& result) const
{
    // On ajoute la salle actuelle dans le chemin
    path.push_back(current);

    // Si on est arrivé au dortoir,
    // alors on a trouvé un chemin complet
    if (current == dortoir)
    {
        // On sauvegarde une copie du chemin trouvé
        result.push_back(path);

        // On enlève la salle avant de revenir en arrière
        path.pop_back();

        return;
    }

    // On cherche les salles voisines de la salle actuelle
    auto it = adjacency.find(current);

    // Si la salle n’a pas de voisins, on s’arrête
    if (it == adjacency.end())
    {
        path.pop_back();
        return;
    }

    // On explore chaque salle voisine
    for (const std::string& voisin : it->second)
    {
        // On vérifie si on a déjà visité cette salle
        // (pour éviter de tourner en boucle)
        bool dejaVisite = false;

        for (const std::string& salle : path)
        {
            if (salle == voisin)
            {
                dejaVisite = true;
                break;
            }
        }

        // Si déjà visité, on ignore cette salle
        if (dejaVisite)
            continue;

        // On explore cette nouvelle salle
        dfsHelper(voisin, path, result);
    }

    // Backtracking :
    // on enlève la salle actuelle pour tester d'autres chemins
    path.pop_back();
}