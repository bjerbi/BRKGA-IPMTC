#ifndef KTNS_H
#define KTNS_H

#include <iostream>
#include <vector>
using namespace std;
long KTNS(const vector<int>processos, bool debug=false) {
	extern std::vector<std::vector<int>> matrixFerramentas;
	extern unsigned t; // ferramentas
	extern int c; // capacidade do magazine
	vector<int> carregadas(t,0);
	int u=0; // ferramentas no magazine
	int prioridades[t][processos.size()];
	int magazine[t][processos.size()];

	if (debug) {
	std::cout << std::endl << "Matriz de Ferramentas no KTNS" << std::endl;
			for (unsigned j = 0; j<t; j++){
				for (unsigned i = 0; i<8; ++i){
					std::cout<<matrixFerramentas[j][i] << " ";
				}
				std::cout<<std::endl;
			}
			std::cout << " --------------------- " <<std::endl;
	std::cout << "Processos" << std::endl;
	for (unsigned i =0; i<processos.size(); i++) {
		std::cout<<processos[i] << " ";
	}
	std::cout << endl;
	std::cout << endl;
	}

	for (unsigned j=0; j<t; j++) {
		carregadas[j]=matrixFerramentas[j][processos[0]];
		if (matrixFerramentas[j][processos[0]]==1)
			++u;

		for (unsigned i =0; i<processos.size(); i++) {
				magazine[j][i] = matrixFerramentas[j][processos[i]];
				if (debug) {
					cout << magazine[j][i] << " ";
				}
			}
			if (debug) {
			 cout << endl;
			}
	}
	// Preenche a matriz de prioridades
	for (unsigned i=0; i<t; ++i){
		for (unsigned j=0; j < processos.size(); ++j){
			if (magazine[i][j]==1)
				prioridades[i][j] = 0;
			else {
				int proxima = 0;
				bool usa = false;
				for (unsigned k=j+1;k<processos.size();++k){
					++proxima;
					if (magazine[i][k]==1){
						usa = true;
						break;
					}
				}
				if (usa)
					prioridades[i][j]=proxima;
				else
					prioridades[i][j]=-1;
			}
		}
	}
	if (debug) {

	for (unsigned j=0; j<t; j++) {
		for (unsigned i =0; i<processos.size(); i++) {
				cout << prioridades[j][i] << " ";
			}
			cout << endl;
	}

	cout << "Ferramentas carregadas: " << endl;
	for (unsigned j=0; j<t; j++) {
		if (carregadas[j]==33) exit(0);
				cout << carregadas[j] << endl;
	}
	}


	// Calcula as trocas
	if (debug) {
	 cout << u << " carregadas na primeira tarefa" << endl;
	}
	int trocas = 0;
	for (unsigned i=1; i<processos.size(); ++i) {
		for (unsigned j=0; j<t; ++j){
			if ((magazine[j][i]==1) && (carregadas[j]==0)){
				carregadas[j]=1;
				++u;
			}
		}
		if (debug) {
			cout << u << " Ferramentas carregadas" << endl;
		}
		while (u>c){
			int maior = 0;
			int pMaior = -1;
			for (unsigned j=0; j<t; ++j) {
				if (magazine[j][i]!=1){ // Ferramenta não utilizada pelo processo atual
					if ((carregadas[j]==1) && (prioridades[j][i] == -1)) { // Essa ferramenta não será mais utilizada e é um excelente candidato a remoção
						pMaior = j;
						break;
					} else {
						if ((prioridades[j][i]>maior) && carregadas[j]==1) {
							maior = prioridades[j][i];
							pMaior = j;
						}
					}
				}
			}
			carregadas[pMaior] = 0;
			if (debug) {
				cout << "Retirou " << i << ":" << pMaior << endl;
			}
			--u;
			++trocas;
			if (debug) {
				cout << trocas << " trocas " << endl;
			}
		}
		if (debug) {

		cout << "Ferramentas carregadas: " << endl;
		for (unsigned j=0; j<t; j++) {
				cout << carregadas[j] << endl;
		}
	}
	}
	if (debug) {
	 cout << ": " << trocas << "trocas" << endl;
	}
	return trocas;
}
double completionTime(std::vector<unsigned> tProcessamento, std::vector<int >& tarefas){
	extern unsigned tempoTroca;
	double tPr = 0;
	for (std::vector<int>::const_iterator i = tarefas.begin(); i!=tarefas.end(); ++i)
		tPr+= tProcessamento[*i];
	long nTrocas = KTNS(tarefas);
	double tTrocas = nTrocas*tempoTroca;
	return (tPr+tTrocas);
}

#endif
