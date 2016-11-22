/*!
   \file Ip.h
   \brief Different outils de gestion d'adresse IP
   \author BAYLE.F
   \date 12/05/2016
*/

#ifndef _IP_
#define _IP_

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include <math.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <net/if.h>

#include <iostream>
#include <string.h>

// #include "StringUtils.cc"

typedef struct netstru
{
  char network[4];
  char broadcast[4];

  char first_host[4];
  char last_host[4];

  char mask[4];
  char wildcard[4];
}netstru;

class IP
{
  private:
    char _fullValue[16];  //ex : 192.168.X.X
    char _bytes[4]; // repartition des donnees :  00000000 [0] 00000000 [1] 00000000 [2] 00000000 [3]

    void explodeIn_bytes(char adress[16], char output[4]); //Desassemble la chaine de caractertes de l'adresse IP pour la stocker bytes par bytes dans _bytes

    char * decimal_to_binary(int decimal, int sizebyte);

    void printbincharpad(char c);

    void builder(const char * ipvalue);

    static std::string _INTERFACE;

  public:
    IP (const char * ipvalue);
    IP ();  //Constructeur automatique
    netstru determine_network(const char * mask);

    int binary_octal(char n);
    static bool ipControl(const char * ip);  //Verifie si la chaine ip est bien une adresse IP (IPV4 pour le moment)

    /*Methode pour recuperer des information reseaux sur l'hote*/
    static void INTERFACE(const char * interface);  //Permet de modifier l'interface (eth0, wlan0, etc..) qui sera analysee pour le mode auto
    static void get_IP(char retour[16]);
    static void get_MASK(char retour[16]);
    static void get_MAC(char retour[18]);

};


#endif
