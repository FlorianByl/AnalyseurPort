#include "Ip.h"
#include <unistd.h>

std::string IP::_INTERFACE = "eth0";

IP::IP(const char * ipvalue)
{
  builder(ipvalue);
}

/*!
   \brief Constructeur

   Constructeur en mode automatique, cad lorsqu'on veux analyser l'adresses
   ip de la machine hote du programme.
*/
IP::IP()
{
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, IP::_INTERFACE.c_str(), IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    /* display result */
    char * addr = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    builder(addr);
}

/*!
   \brief Découpe l'adresse IP pour pouvoir la travailler
   \param ipvalue : Adresse ip a utiliser
*/
void IP::builder(const char * ipvalue)
{
    //On verifie que l'adresse IP en est bien une
    if(!ipControl(ipvalue))
    {
      perror("Error in IP");
      return;
    }

    //On enregistre l'adresse ip
    memset(_fullValue, '\0', sizeof(_fullValue));
    strncpy(_fullValue, ipvalue, 16);


    explodeIn_bytes(_fullValue, _bytes);
}


/*!
   \brief Retourne l'adresse IP de la machine
   \param retour : Adresse ip de la machine
*/
/*static*/ void IP::get_IP(char retour[16])
{
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, IP::_INTERFACE.c_str(), IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    /* display result */
    strncpy(retour, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr), IFNAMSIZ-1);
    // strncpy()
}

/*!
   \brief Retourne le masque de la machine
   \param retour : Masque de la machine
*/
/*static*/ void IP::get_MASK(char retour[16])
{
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, IP::_INTERFACE.c_str(), IFNAMSIZ-1);

    ioctl(fd, SIOCGIFNETMASK, &ifr);

    close(fd);

    /* display result */
    strncpy(retour, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr), IFNAMSIZ-1);
    // strncpy()
}

/*!
   \brief Retourne l'adresse MAC de la machine
   \param retour : Adresse MAC de la machine
*/
/*static*/ void IP::get_MAC(char retour[18])
{
  memset(retour, '\0', 18);
  int fd;
  struct ifreq ifr;

  fd = socket(AF_INET, SOCK_DGRAM, 0);

  /* I want to get an IPv4 IP address */
  ifr.ifr_addr.sa_family = AF_INET;

  /* I want IP address attached to "eth0" */
  strncpy(ifr.ifr_name, IP::_INTERFACE.c_str(), IFNAMSIZ-1);

  ioctl(fd, SIOCGIFHWADDR, &ifr);

  close(fd);

  std::string mac=(const char*)ifr.ifr_hwaddr.sa_data;

  sprintf(retour, "%02X:%02X:%02X:%02X:%02X:%02X\n",
    mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
}

/*!
   \brief Permet de modifier l'interface reseau lors du mode automatique
   \param interface : ex : eth0, wlan0
   \pre Mode automatique

   Permet de modifier l'interface reseau analysee lorsque le mode automatique
   est demandé.
*/
/*static*/ void IP::INTERFACE(const char * interface)
{
    IP::_INTERFACE = interface;
}

/*!
   \brief Control l'authenticite d'une adresse IP
   \param ip : IP a verifier
   \return TRUE : Si adresse OK
   \return FALSE : Si adresse NON OK
*/
/*static*/bool IP::ipControl(const char * ip)
{
  //Une adresse IP peux faire au minimum 7 caractère (ex : 0.0.0.0) ou au
  //maximum 15 caracteres (255.255.255.255). On verifie donc la taille

  int ip_size = strlen(ip);
  if( (ip_size < 7) || (ip_size > 15) )
  {
    errno = EFAULT;
    return 0;
  }

  //On verifie grace à inet si l'adresse IP est valide (Tips and tricks found
  //on internet !)

  struct sockaddr_in sa;
  char str[INET_ADDRSTRLEN];
  if(inet_pton(AF_INET, ip, &(sa.sin_addr)) == 0)
  {
    errno = EFAULT;
    return 0;
  }

  return 1;
}


/*!
   \brief Convertie une adresse sous forme de chaine en 4 octets
   \param address : Adresse IP sous forme de chaine de caractere
   \param output : Tableau de sortie qui contient les 4 octet de l'adresse
*/
void IP::explodeIn_bytes(char adress[16], char output[4])
{
  //On prepare les variables qui contiendrons, les octets puis ensuite les bits
  int bytes[4]; memset(bytes, 0, sizeof(int)*4);

  //On utilise la fonction strtok pour exploser la chaine
  const char target[2] = "."; //On cible le point pour exploser la chaine
  char * token;

  //Recupere la premiere coupe
  token = strtok(adress, target);
  int i = 0;  //Servira a stocker les coupes

  //Tant qu'on trouve d'autres coupes, on stock
  while(token != NULL)
  {
    sscanf(token, "%d", &bytes[i]);
    ++i;

    token = strtok(NULL, target);
  }

  //A ce moment, bytes contient pour chaque entrées de son tableau, un octet
  //sous forme decimale correspondant a l'adresse IP


  for(int j = 0 ; j < i; ++j) //Parcour chaque octet
  {
    output[j] = bytes[j];
  }

}

/*!
   \brief Conversion Decimal vers binaire
   \param decimal : Chiffre decimal
   \param sizebyte : Nombre de bits demandé en sortie
   \return Binaire du decimal

   Thx to http://www.programmingsimplified.com/ for this code
*/
char * IP::decimal_to_binary(int decimal, int sizebyte)
{
  /*
    Thx to http://www.programmingsimplified.com/ for this code
  */
  int c, d, count;
  char *pointer;

  count = 0;
  pointer = (char*)malloc(32+1);

  if ( pointer == NULL )
    exit(EXIT_FAILURE);

  for ( c = sizebyte-1 ; c >= 0 ; c-- )
  {
    d = decimal >> c;

    if ( d & 1 )
       *(pointer+count) = 1 + '0';
    else
       *(pointer+count) = 0 + '0';

    count++;
  }
  *(pointer+count) = '\0';

  return  pointer;
}


/*!
   \brief Affiche un octet sur la sortie standard
   \param c : Octet a afficher
*/
void IP::printbincharpad(char c)
{
    for (int i = 7; i >= 0; --i)
    {
        putchar( (c & (1 << i)) ? '1' : '0' );
    }
    putchar('\n');
}

/*!
   \brief Calcul le nombre d'ip possible sur un reseau en fonction du mask
   \param mask : Masque de sous reseau
   \return Structure contenant les informations liées a un reseau
*/
netstru IP::determine_network(const char * mask)
{

  //Structure de retour
  netstru retour;

  //On verifie que le masque en est bien un
  if(!ipControl(mask))
  {
    perror("Error in Mask");
    netstru er;
    return er;
  }

  //On recupere la masque sous forme binaire
  char c_mask[16]; //Convert mask in unsigned char
  memset(c_mask, '\0', sizeof(c_mask));
  strncpy(c_mask, mask, 16);

  //Recupere l'adresse en 4 octets
  char bytes[4];
  explodeIn_bytes(c_mask, bytes);

  //Sauvegarde du mask dans la structure de retour
  memset(retour.mask, '\0', sizeof(retour.mask));
  strncpy(retour.mask, bytes, 4);

  //Applique le ET logique entre masque et adresse pour obtenir reseau
  char network[4];
  for(int i = 0; i < 4; ++i)
    network[i] = bytes[i] & _bytes[i];


  // for(int i = 0; i < 4; ++i)
  //   printbincharpad(bytes[i]);

  //Sauvegarde de l'adresse reseau dans la structure de retour
  memset(retour.network, '\0', sizeof(retour.network));
  strncpy(retour.network, network, 4);


  //Pour obtenir l'adresse broadcast il faut faire un OU logique entre
  //[le masque inversé] et [l'adresse reseau]. On commence par recuperer
  //Le masque inversé

  char wildcard[4];
  for(int i = 0; i < 4; ++i){
    wildcard[i] = 0xFF - bytes[i];
    retour.wildcard[i] = wildcard[i]; //Sauvegarde
  }


  //On effectue maintenant le calcul du broadcast
  char broadcast [4];
  for(int i = 0 ; i< 4; ++i){
    broadcast[i] = wildcard[i] | network[i];
    retour.broadcast[i] = broadcast[i]; ////Sauvegarde
  }



  //Recupere la premiere adresse du reseau
  char first[4];
  for(int i = 0 ; i < 4; ++i)
    first[i] = network[i];
  first[3] = 1u << 0;
  for(int i = 0 ; i < 4; ++i)
    retour.first_host[i] = first[i];  //Sauvegarde


  //Recupere la derniere adresse du reseau
  char last[4];
  for(int i = 0 ; i < 4; ++i)
    last[i] = broadcast[i];
  last[3] = ~ (1u << 0);
  for(int i = 0 ; i < 4; ++i)
    retour.last_host[i] = last[i];  //Sauvegarde



  /***  Verifie l'integralite des données ***/
  // std::cout << "Reseau : " << binary_octal(retour.network[0]) << "." << binary_octal(retour.network[1]) << "." << binary_octal(retour.network[2]) << "." << binary_octal(retour.network[3]) << std::endl;
  // std::cout << "Masque : " << binary_octal(retour.mask[0]) << "." << binary_octal(retour.mask[1]) << "." << binary_octal(retour.mask[2]) << "." << binary_octal(retour.mask[3]) << std::endl;
  // std::cout << "Masque inverse : " << binary_octal(retour.wildcard[0]) << "." << binary_octal(retour.wildcard[1]) << "." << binary_octal(retour.wildcard[2]) << "." << binary_octal(retour.wildcard[3]) << std::endl;
  //
  // std::cout << "Premier hote : " << binary_octal(retour.first_host[0]) << "." << binary_octal(retour.first_host[1]) << "." << binary_octal(retour.first_host[2]) << "." << binary_octal(retour.first_host[3]) << std::endl;
  // std::cout << "dernier hote : " << binary_octal(retour.last_host[0]) << "." << binary_octal(retour.last_host[1]) << "." << binary_octal(retour.last_host[2]) << "." << binary_octal(retour.last_host[3]) << std::endl;
  // std::cout << "Broadcast : " << binary_octal(retour.broadcast[0]) << "." << binary_octal(retour.broadcast[1]) << "." << binary_octal(retour.broadcast[2]) << "." << binary_octal(retour.broadcast[3]) << std::endl;

  return retour;


}

/*!
   \brief Binaire vers decimal
   \param c : octet
   \return decimal du binaire
*/
int IP::binary_octal(char n)
{
  int result = 0;
  int init_pow = 128;
  for(int i = 7, divi = 1; i >= 0; --i, divi*=2 )
  {
    if((n >> i) & 1u){
      result += init_pow / divi;
    }

  }
  return result;
}
