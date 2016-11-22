/*!
   \file PortAnalyser.h
   \brief Classe qui permet l'analyse de port d'un peripherique sur reseau
   \author BAYLE.F
   \date 12/05/2016
*/

#ifndef _PA_
#define _PA_

#include <iostream>
    using namespace std;

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <map>
#include <thread>
#include <mutex>

#include "StringUtils.h"
#include "Ip.h"

typedef struct multi_arg
{
  char networkIp[16];
  int port;
}muli_arg;

class PortAnalyser
{
  private:
    static bool networkInputDetected(const char * ip);
    static std::vector< std::pair <string, bool> >  _mainOutput;
    static std::mutex barrier;
    static std::mutex barrier_sock;
  public:
    static bool checkPort(const char * ip, int port); //Verifie si un port est ouvert a un adresse donnée
    static int checkPortOnMultipleTarget(const char * network_ip, const char * mask, int port, std::vector< std::pair <string, bool> >  &output);  //Test un port donné sur l'enssemble d'un reseaux
    static int checkPortOnMultipleTarget(int port, std::vector< std::pair <string, bool> >  &output);  //Test un port donné sur l'enssemble d'un reseaux

    static std::vector<std::string> listOfaddress(const char * networkIp, const char * mask); //Renvoi la liste des adresse pouvant existée sur le reseaux networkIp au masque mask


};

#endif
