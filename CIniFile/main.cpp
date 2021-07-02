#include <iostream>

#include "CIniFile.h"

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main( int argc, char** argv) {
	
  CIniFile myIniFile( (char*)"MyFile.ini");
  
  std::cout << "LocalNumb=" << 41 << std::endl;
  myIniFile.WriteInteger( (char*)"Settings", (char*)"LocalNumb", 41);

  std::cout << "LocalTemp=" << 36.7 << std::endl;
  myIniFile.WriteFloat( (char*)"Settings", (char*)"LocalTemp", 36.7);

  std::cout << "LocalName=" << "Silvia" << std::endl;
  myIniFile.WriteString ( (char*)"Settings", (char*)"LocalName", (char*)"Silvia");

  std::cout << "Debug=" << true << std::endl;
  myIniFile.WriteBoolean( (char*)"Settings", (char*)"Debug", true);
  
  int localNumber = myIniFile.ReadInteger( (char*)"Settings", (char*)"LocalNumb", 13);	
  std::cout << "LocalNumb=" << localNumber << std::endl;
  
  float localTemperature = myIniFile.ReadFloat( (char*)"Settings", (char*)"LocalTemp", 33.5);	
  std::cout << "LocalTemp=" << localTemperature << std::endl;
  
  char* szName = myIniFile.ReadString( (char*)"Settings", (char*)"LocalName", (char*)"Suzy");	
  std::cout << "LocalName=" << szName << std::endl;
  
  bool debug = myIniFile.ReadBoolean( (char*)"Settings", (char*)"Debug", true);	
  std::cout << "Debug=" << debug << std::endl;
  
  return 0;
}
