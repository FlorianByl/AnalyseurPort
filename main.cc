#include <iostream>
    using namespace std;

#include "PortAnalyser.h"
#include "Ip.h"


int main(int argc, char const *argv[])
{
  int port;
  if(argc <= 1){
    std::cout << "Veuillez indiquer le port" << std::endl;
    return -1;
  }

  sscanf(argv[1], "%i", &port);
  std::vector< std::pair <string, bool> > result;
  PortAnalyser::checkPortOnMultipleTarget(port, result);

  for(int i = 0; i < result.size(); ++i)
    if(result[i].second)
      std::cout << "adresse : " << result[i].first << "   |   resultat : " << result[i].second << std::endl;


  return 0;
}
