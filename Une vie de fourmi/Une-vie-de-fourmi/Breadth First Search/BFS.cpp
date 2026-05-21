// SECTION BFS (Breadth First Search)
// BFS explore le graphe niveau par niveau.
//
// Contrairement au DFS qui suit un chemin jusqu'au bout,
// le BFS explore toutes les possibilités proches avant d'aller plus loin.
//
// Ici, on cherche tous les chemins possibles
// entre le vestibule (départ) et le dortoir (arrivée).

std::vector<std::vector<std::string>> Anthill::findAllPathsBFS() const
{
    // Liste finale qui contient tous les chemins trouvés
    // Chaque chemin est une suite de salles
    std::vector<std::vector<std::string>> resultats;

    // File d'attente (queue)
    // Elle contient des chemins COMPLETS (pas juste des salles)
    // Exemple : [A], [A->B], [A->C->D]
    std::queue<std::vector<std::string>> file;

    // On commence avec un chemin contenant uniquement le vestibule
    file.push({ vestibule });

    // Tant qu'il reste des chemins à explorer
    while (!file.empty())
    {
        // On prend le premier chemin de la file
        std::vector<std::string> chemin = file.front();

        // On le retire de la file (il va être traité)
        file.pop();

        // On récupère la dernière salle du chemin
        // (la position actuelle de la "fourmi")
        std::string actuel = chemin.back();

        // Si on est arrivé au dortoir
        // alors on a trouvé un chemin complet
        if (actuel == dortoir)
        {
            // On sauvegarde ce chemin dans les résultats
            resultats.push_back(chemin);

            // On passe au chemin suivant
            continue;
        }

        // On cherche les salles voisines de la salle actuelle
        auto it = adjacency.find(actuel);

        // Si la salle n'existe pas ou n'a pas de voisins
        if (it == adjacency.end())
            continue;

        // On explore toutes les salles voisines
        for (const std::string& voisin : it->second)
        {
            // -------------------------------
            // Vérification des cycles
            // -------------------------------
            // On vérifie si le voisin est déjà dans le chemin actuel
            // pour éviter de revenir en arrière en boucle infinie

            bool dejaVisite = false;

            // On parcourt tout le chemin actuel
            for (const std::string& n : chemin)
            {
                if (n == voisin)
                {
                    dejaVisite = true;
                    break;
                }
            }

            // Si la salle n'a pas encore été visitée
            if (!dejaVisite)
            {
                // On crée un nouveau chemin basé sur l'ancien
                std::vector<std::string> nouveauChemin = chemin;

                // On ajoute la nouvelle salle au chemin
                nouveauChemin.push_back(voisin);

                // On ajoute ce nouveau chemin dans la file
                // pour l'explorer plus tard
                file.push(nouveauChemin);
            }
        }
    }

    // On retourne tous les chemins trouvés
    return resultats;
}


// -------------------------------------------------------------------


// Fonction qui lance le BFS et affiche les résultats

void Anthill::simulate()
{
    // On récupère tous les chemins trouvés par BFS
    auto chemins = findAllPathsBFS();

    // Si aucun chemin n'est trouvé
    if (chemins.empty())
        throw std::runtime_error("BFS : aucun chemin trouvé !");

    // Affichage du nombre de chemins trouvés
    std::cout << "[BFS] Chemins trouvés : " << chemins.size() << "\n";

    // On parcourt tous les chemins trouvés
    for (size_t i = 0; i < chemins.size(); i++)
    {
        std::cout << "  Chemin " << i + 1 << " : ";

        // On affiche chaque salle du chemin
        for (size_t j = 0; j < chemins[i].size(); j++)
        {
            std::cout << chemins[i][j];

            // On ajoute une flèche entre les salles
            if (j + 1 < chemins[i].size())
                std::cout << " -> ";
        }

        std::cout << "\n";
    }

    // On lance la simulation finale avec tous les chemins BFS
    runSimulation(chemins, "BFS");
}