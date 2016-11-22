#include "StringUtils.h"

StringUtils::StringUtils(){}
StringUtils::~StringUtils(){}

/**
 *    Permet de retourner un tableau de chaine crées a partir de la chaine pattern.
 *    La chaine pattern (après recopie) est scindée en plusieurs morceaux. La découpe
 *    se fait au niveau des occurences.
 */
/*static*/std::vector<std::string> StringUtils::explode(std::string pattern, const char * occurence)
{
    std::size_t last_pos = 0;
    std::size_t found;

    //Vector de retour
    std::vector<std::string> found_string;

    //Sauvegarde le pattern dans une chaine de classe string
    std::string cible = pattern;


    //Algo principal
    found = cible.find_first_of(occurence, last_pos);
    while( found != std::string::npos)
    {
        std::string temporaire = "";
        for (int i = last_pos; i < found; i++)
        {
            temporaire += cible[i];
        }
        found_string.push_back(temporaire);

        last_pos = found+1;
        found = cible.find_first_of(occurence, last_pos);

    }

    //Construction de la fin de chaine
    std::string temporaire = "";
    if(found == std::string::npos)
        for(int i = last_pos; i < cible.length(); ++i)
            temporaire += cible[i];
    found_string.push_back(temporaire);

    return found_string;
}
