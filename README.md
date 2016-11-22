***Analyseur de ports***

Ce projet doit aboutir à une librairie qui se mettra à jour dans le temps et qui
aura pour fonction d'analyser les ports sur un réseau donné (ou sur le réseau de
la machine exécutant le programme).

Modifier "IP::_INTERFACE" dans Ip.cc afin de modifier l'interface réseau.


Delock le nombre max de fichiers : ulimit -n 4096



***IDEES***

Ajouter des paramètres de type -o -d etc
Possibilités avec les paramètres :

    -f xxxx.txt : Parse un fichier contenant des adresses ip et les tests
    -p 21 : Parse le port 21 (Si omis, alors parse tous les ports)
    -o xxx.txt : Fichier de sortie (sinon console mais peut être illisible si trop d'adresses)



Ajouter une methode qui test tous les ports de chaque machines.
