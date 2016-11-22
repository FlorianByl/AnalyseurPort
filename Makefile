LINK+=-std=c++11 -pthread

all:comp clean

comp:stringutils ip portanalyser SettingManager main
	g++ *.o $(LINK)

main:main.cc
	g++ -c main.cc -o main.o $(LINK)

portanalyser:PortAnalyser.cc
	g++ -c PortAnalyser.cc -o portanalyser.o $(LINK)

stringutils:StringUtils.cc
	g++ -c StringUtils.cc -o stringutils.o $(LINK)

ip:Ip.cc
	g++ -c Ip.cc -o ip.o $(LINK)

SettingManager:SettingManager.cc
	g++ -c SettingManager.cc -o setman.o $(LINK)

clean:
	rm *.o
