#include "PortAnalyser.h"

std::vector< std::pair <string, bool> >  PortAnalyser::_mainOutput;
std::mutex PortAnalyser::barrier;
std::mutex PortAnalyser::barrier_sock;

/*static*/ bool PortAnalyser::checkPort(const char * ip, int port)
{
  // std::cout << "Thread lancé" << std::endl;
  struct timeval timeout;
  timeout.tv_sec = 1;
  timeout.tv_usec = 200000;


  /*** Création du socket pour l'adresse et le port cible ***/
  // std::lock_guard<std::mutex> block_threads_until_finish_this_job(PortAnalyser::barrier_sock);
  int sock = socket(AF_INET, SOCK_STREAM, 0); //Creation du socket de type TCP sans parametres particuliers
  // fcntl(sock, F_SETFL, O_NONBLOCK); //On ajoute au fd le paramètre non bloquant pour eviter le freeze du programme principale
  if(sock == -1)  //En cas d'erreur lors de la creation du socket on preferera quitter la methode plutot que tu crasher le programme
  {
      perror("can't create socket");
      return 0;
  }

  if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    perror("setsockopt failed\n");

  if (setsockopt (sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    perror("setsockopt failed\n");


  struct hostent *hostinfo = NULL;
  sockaddr_in sin = { 0 }; /* initialise la structure avec des 0 */
  const char *hostname = ip; //Adresse cible

  hostinfo = gethostbyname(hostname); /* on récupère les informations de l'hôte auquel on veut se connecter */
  if (hostinfo == NULL) /* l'hôte n'existe pas */
  {
      shutdown(sock, SHUT_WR);
      close(sock);
      return 0;
  }

  sin.sin_addr = *(in_addr *) hostinfo->h_addr; /* l'adresse se trouve dans le champ h_addr de la structure hostinfo */
  sin.sin_port = htons(port); /* on utilise htons pour le port */
  sin.sin_family = AF_INET;



  /*** Lancement de la connection  ***/
  if(connect(sock,(sockaddr *) &sin, sizeof(sockaddr)) >= 0)
  {
    //La connection à réussie si on entre
    shutdown(sock, SHUT_WR);
    close(sock);
    std::lock_guard<std::mutex> block_threads_until_finish_this_job(PortAnalyser::barrier);
    _mainOutput.push_back(std::pair <string, bool>(ip, true));
    return 1;
  }
  shutdown(sock, SHUT_WR);
  close(sock);
  std::lock_guard<std::mutex> block_threads_until_finish_this_job(PortAnalyser::barrier);
  _mainOutput.push_back(std::pair <string, bool>(ip, false));
  return 0;
}


/*!
   \brief //Verifie si la chaine passée en parametre est une adresse ip
          de reseau (finissant par 0)
   \param ip : Adresse IP
   \return True : La chaine passée en paramètre est une adresse IP
   \return False : La chaine passée en paramètre n'est pas une adresse IP
*/
/*static*/ bool PortAnalyser::networkInputDetected(const char * ip)
{
  std::vector<std::string> networkInOctal = StringUtils::explode(ip, ".");

  for(int i = 0; i < networkInOctal.size(); ++i)
  {
    if((atoi(networkInOctal[i].c_str()) == 0) && i == networkInOctal.size() - 1)
      return 1;
  }

  return 0;
}

/*!
   \brief Verifie si un port repond sur une liste d'adresses
   \param network_ip : Adresse IP
   \param mask : Masque de sous reseau
   \param port : Port a analyser
   \param output : Vecteur de sortie
   \return 0
*/
/*static*/ int PortAnalyser::checkPortOnMultipleTarget(const char * network_ip, const char * mask, int port, std::vector< std::pair <string, bool> >  &output)
{

  int limite = 370;
  std::vector<string> netTar = PortAnalyser::listOfaddress(network_ip, mask);
  std::vector<std::thread *> threads;

  for(int i = 0, k = 0; i < netTar.size(); ++i, ++k)
  {
    if(k >= limite)
    {
      for(int l = i-k; l < i; ++l)
      {
        if(threads[l]->joinable())
          threads[l]->join();
      }
      k=0;
    }
    usleep(2000);
    threads.push_back(new std::thread(PortAnalyser::checkPort, netTar[i].c_str(), port));
  }

  for(int i = 0; i < threads.size(); ++i)
  {
    if(threads[i]->joinable())
      threads[i]->join();
  }
  output = _mainOutput;
  _mainOutput.clear();
  return 0;


}


/*!
   \brief Verifie si un port repond sur une liste d'adresse IP (mode auto)
   \param port : Port a analyser
   \param output : Vecteur de sortie
   \return 0
*/
/*static*/ int PortAnalyser::checkPortOnMultipleTarget(int port, std::vector< std::pair <string, bool> >  &output)
{
    char ip[15], mask[15];
    int limite = 370;
    IP::get_IP(ip); IP::get_MASK(mask);
    std::vector<string> netTar = PortAnalyser::listOfaddress(ip, mask);
    std::vector<std::thread *> threads;

    for(int i = 0, k = 0; i < netTar.size(); ++i, ++k)
    {
      if(k >= limite)
      {
        for(int l = i-k; l < i; ++l)
        {
          if(threads[l]->joinable())
            threads[l]->join();
        }
        k=0;
      }
      threads.push_back(new std::thread(PortAnalyser::checkPort, netTar[i].c_str(), port));
    }

    for(int i = 0; i < threads.size(); ++i)
    {
      if(threads[i]->joinable())
        threads[i]->join();
    }
    output = _mainOutput;
    _mainOutput.clear();
    return 0;
}




/*!
   \brief Renvoie une liste d'adresse ip en fonction d'un ip et masque
   \param networkIp : Adresse IP
   \param mask : Masque de sous reseau
   \return vecteur d'adresse ip

   Methode qui permet grace a une adresse ip et un masque de sous reseau
   associé, de recuperer la liste des toutes les adresses ip possible sur
   le reseau.
*/
/*static*/ std::vector<std::string> PortAnalyser::listOfaddress(const char * networkIp, const char * mask)
{
  //Variable de sortie
  std::vector<std::string> retour;


  IP * ip = new IP(networkIp);
  netstru net_info = ip->determine_network(mask);

  //On recupere l'adresse du premier hote sous forme de chaine
  char first[16];
  sprintf(first, "%d.%d.%d.%d", ip->binary_octal(net_info.first_host[0]), ip->binary_octal(net_info.first_host[1]), ip->binary_octal(net_info.first_host[2]),ip->binary_octal(net_info.first_host[3]) );

  //On recupere l'adresse du dernier hote sous forme de chaine
  char last[16];
  sprintf(last, "%d.%d.%d.%d", ip->binary_octal(net_info.last_host[0]), ip->binary_octal(net_info.last_host[1]), ip->binary_octal(net_info.last_host[2]),ip->binary_octal(net_info.last_host[3]) );


  //On va maintenant incrementer l'ip jusqu'a ip max et les stocker
  //On se sert de la structure in_addr pour incrementer tranquillement
  in_addr adr;
  adr.s_addr = inet_addr(first);
  retour.push_back(inet_ntoa(adr));
  while(strcmp (inet_ntoa(adr),last) != 0)
  {
    adr.s_addr  = htonl(ntohl(adr.s_addr) + 1);
    retour.push_back(inet_ntoa(adr));
  }


  return retour;
}
