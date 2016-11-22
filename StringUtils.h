/*
*	 ____   _            _    _            _      _____  _           _  _
*	|    \ |_| ___  ___ |_| _| | ___  ___ | |_   |   __|| |_  _ _  _| ||_| ___
*	|  |  || ||_ -||_ -|| || . || -_||   ||  _|  |__   ||  _|| | || . || || . |
*	|____/ |_||___||___||_||___||___||_|_||_|    |_____||_|  |___||___||_||___|
*
*							Projet Andhrimnir (Engine)
*
*						     Auteur 		: BAYLE.f
*						     Date 			: 09/01/2016
*			    	 Cette œuvre est mise à disposition selon
*			    	 les termes de la Licence Creative Commons  :
*			     	 Attribution - Pas d'Utilisation Commerciale
*				     Pas de Modification 4.0 International.
*/

#ifndef STRINGUTILS_
#define STRINGUTILS_

#include <iostream>
#include <string>
#include <vector>

namespace utils
{

class StringUtils
{
    /*ATTRIBUTS*/
    private:
    protected:
    public:


    /*Methodes*/
    private:
        StringUtils ();
        virtual ~StringUtils ();
    protected:
    public:
        static std::vector<std::string> explode(std::string pattern, const char * occurence);
};

}

using namespace utils;


#endif //STRINGUTILS_
