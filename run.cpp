#include <dirent.h>
#include <cstdlib>
#include <string>
#include <fstream>

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	if (argc != 3) {
        std::cerr << "Diretório com as instâncias não foi informado." << std::endl;
        exit (1);
    }
	std::string nomeDir = argv[1];
	int repeticoes = std::stoi(argv[2]);
	DIR *dir = 0;
    struct dirent *entrada = 0;
    unsigned char isFile = 0x8;

    dir = opendir (nomeDir.c_str());

    if (dir == 0) {
        std::cerr << "Nao foi possível abrir diretorio com as instâncias." << std::endl;
        exit (1);
    }
	// Pego todas as instâncias do diretório
	while ((entrada = readdir (dir))){
    if (entrada->d_type == isFile){
			// Executo o programa N vezes
      std::string xF = nomeDir + "solucoes/BRKGA_SOLUCAO_"+entrada->d_name + "_10";
      const char * x = xF.c_str();
      FILE *TFile;// = fopen(x,"r");
      //if (!TFile){ // Ignorar as instancias já executadas
  			for (int i=1;i<=repeticoes;++i) {
  				std::stringstream convert;
  				std::cout << i << " Execucao: " << nomeDir+entrada->d_name << std::endl;
  				convert << i;
          std::string xF = nomeDir + "solucoes/BRKGA_SOLUCAO_"+entrada->d_name +  "_" + convert.str();
          x = xF.c_str();
          std::ifstream existeInstancia;
          existeInstancia.open(xF);
          if (!existeInstancia.is_open()){
          //TFile = fopen(x,"r");
          //if (!TFile){ // Ignorar as instancias já executadas
    				std::string cmd = "./samplecode <" + nomeDir+entrada->d_name + " >" + nomeDir + "solucoes/BRKGA_SOLUCAO_"+entrada->d_name + "_" + convert.str();
    				const char * c = cmd.c_str();
    				int s = system(c);
          } else{
            existeInstancia.close();
            std::cout <<  "já executado. " << std::endl;
          }
  			}
      //} else {
      //  std::cout << nomeDir + "solucoes/BRKGA_SOLUCAO_"+entrada->d_name + " já executado." << std::endl;
      //}
		}
	}
    closedir (dir);

    nomeDir+="solucoes/";
    std::cout << nomeDir;
	dir = opendir (nomeDir.c_str());
	// Gerando o acumulado
	std::ifstream file;
	std::ofstream fileR;
	std::string dado, dado2, dado3, dado4, dado5, dado6, dado7, dado8, dado9, dado10, dado11;
	fileR.open(nomeDir+"BRKGA_RESUMO.txt");
	while ((entrada = readdir (dir))){
        if ((entrada->d_type == isFile)){
			 std::string nomeArq = entrada->d_name;
			 if (nomeArq.compare("BRKGA_RESUMO.txt")!=0){
				 file.open(nomeDir+entrada->d_name, std::ifstream::in);
				 file >> dado;
				 file >> dado2;
         file >> dado3;
         file >> dado4;
         file >> dado5;
         file >> dado6;
         file >> dado7;
         file >> dado8;
         file >> dado9;
         file >> dado10;
         file >> dado11;
				 fileR << entrada->d_name << " " << dado << " " << dado2 << " " << dado3 << " " << dado4 << " " << dado5 << " " << dado6 << " " << dado7 << " " << dado8 << " " << dado9 << " " << dado10 <<  " " << dado11 << std::endl;
				 file.close();
			 }
		}
	}
	fileR.close();
	closedir(dir);
	return 0;
}
