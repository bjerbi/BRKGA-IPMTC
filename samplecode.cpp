#include <dirent.h>
#include <cstdlib>
#include <string>
#include <fstream>

#include <iostream>
#include <vector>
#include <ctime>
#include <ratio>
#include <chrono>

#include "SampleDecoder.h"
#include "MTRand.h"
#include "BRKGA.h"
#define MAXBIT 40

std::vector < std::bitset <MAXBIT> > bitMatrix(202);


std::vector<std::vector<int>> matrixFerramentas;
unsigned n = 0; // tarefas
int m = 0; // máquinas
unsigned t = 0; // ferramentas
unsigned c = 0; // tool magazine
unsigned tempoTroca = 0; // tempo de troca entre duas ferramentas
unsigned contadorUniversal = 0;
unsigned processamento;
std::vector<unsigned>tProcessamento; // tempo de processamento das tarefas

//4626
unsigned p = 1000;	// size of population
const double pe = 0.30;		// fraction of population to be the elite-set
const double pm = 0.25;		// fraction of population to be replaced by mutants
const double rhoe = 0.85;	// probability that offspring inherit an allele from elite parent
const unsigned K = 1;		// number of independent populations
const unsigned MAXT = 4;	// number of threads for parallel decoding
unsigned generation = 1;		// current generation
const unsigned X_INTVL = 100;	// exchange best individuals at every 100 generations
const unsigned X_NUMBER = 2;	// exchange top 2 best
const unsigned MAX_GENS = 100;	// run for 1000 gens
const long unsigned rngSeed = 0;	// seed to the random number generator
MTRand rng(rngSeed);				// initialize the random number generator
const unsigned MAX_VOID = 1000; //  gerações sem melhoria permitidos

unsigned nVoid = 0; // numero de gerações sem melhorias

double tempoBuscas[3]; // IBNS, EFB, ONB
long execucoesBuscas[3];
long melhoriasBuscas[3];
double mediaMelhorias[3];
double primeiroMakespan;
int main(int argc, char* argv[]) {

	using namespace std::chrono;

	// const unsigned n = 100;		// size of chromosomes
  tProcessamento.clear();
	matrixFerramentas.clear();
  cin >> m;
  cin >> n;
  cin >> t;
  cin >> c;
  cin >> tempoTroca;
  p = 5*n;
	for (unsigned i = 0; i<n; ++i){
		cin >> processamento;
		tProcessamento.push_back(processamento);
	}
	// carrega a matrix de ferramentas
	for (unsigned j = 0; j<t; j++){
		std::vector<int> tmpF;
		for (unsigned i = 0; i<n; ++i){
			int tooli = 0;
			cin >> tooli;
			tmpF.push_back(tooli);
		}
		matrixFerramentas.push_back(tmpF);
	}
  for (int i=0;i<3;++i){
    tempoBuscas[i]=0;
    execucoesBuscas[i]=0;
    melhoriasBuscas[i]=0;
    mediaMelhorias[i]=0;
  }

	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	SampleDecoder decoder(tProcessamento);			// initialize the decoder
	// initialize the BRKGA-based heuristic
	BRKGA< SampleDecoder, MTRand > algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT, m);
	double teste_makespan = 9999999999;
  double solucaoInicial = -1;
  primeiroMakespan = -1;
	long teste_geracao = 0;
  high_resolution_clock::time_point t2;
  duration<double> time_span;
	do {
		algorithm.evolve();// evolve the population for one generation
		if((++generation) % X_INTVL == 0) {
			algorithm.exchangeElite(X_NUMBER);	// exchange top individuals
		}
    if (solucaoInicial==-1) solucaoInicial = algorithm.getBestFitness();

    if (algorithm.getBestFitness()  < teste_makespan){
      nVoid = 0;
			teste_geracao = generation;
			teste_makespan = algorithm.getBestFitness();
		} else {
      ++nVoid;
    }

    t2 = high_resolution_clock::now();
  	time_span = duration_cast<duration<double>>(t2 - t1);

	} while (generation < MAX_GENS && nVoid < MAX_VOID && time_span.count()<3600);
	t2 = high_resolution_clock::now();
	time_span = duration_cast<duration<double>>(t2 - t1);
  /*
  cout << "\n\nMakespan: " << algorithm.getBestFitness() << "\n\n";

	cout << "Tempo de execução: " << time_span.count() << " segundos \n\n";
  cout <<endl<< "Melhor geração: " << teste_geracao << endl;
  cout << "Melhor solução" << std::endl;
  */
	vector<double>ch =algorithm.getBestChromosome();

		int maquina = -1;
		int at = 0;
		double tPr = 0.0;
		double makespan = 0.0;
		std::vector < std::pair < double, unsigned > > ranking(ch.size());
		for(unsigned i = 0; i < ch.size(); ++i){
			ranking[i]=std::pair<double,unsigned>(ch[i],i);
		}
		std::sort(ranking.begin(), ranking.end());
		std::vector<int> processos;
		int nTrocas = 0;
		long tTrocas = 0;
		extern unsigned tempoTroca;
    // *************
    std::vector < std::vector <int> > maquinas; // Todas as máquinas
		std::vector < std::pair <double,int> > idx_maquinas; // completionTime, machine
    // *************
		for(std::vector<std::pair<double, unsigned>>::const_iterator i = ranking.begin(); i!=ranking.end(); ++i){
			at = (int) i->first;
			if (maquina==-1){
				maquina = at;
				processos.clear();
			}
			if (maquina==at){
				tPr += tProcessamento[i->second];
				processos.push_back(i->second);
			} else {
				nTrocas=KTNS(processos,false);
				tTrocas = tempoTroca*nTrocas;

				if ((tPr+tTrocas)>makespan){
					makespan = tPr+tTrocas;
				}
        // ***************************
        maquinas.push_back(processos);
        idx_maquinas.push_back(std::pair<double,int>((tPr+tTrocas),maquina));
        // ****************************

    		maquina = at;
				processos.clear();
				tPr = tProcessamento[i->second];
				processos.push_back(i->second);
			}
			 // std::cout << i->first << ":" << i->second << " ";
		}

		nTrocas=KTNS(processos,false);
		tTrocas = tempoTroca*nTrocas;
		if ((tPr+tTrocas)>makespan){
			makespan = tPr+tTrocas;
		}
    // ***************************
    maquinas.push_back(processos);
    idx_maquinas.push_back(std::pair<double,int>((tPr+tTrocas),maquina));

    cout << makespan << " " << time_span.count() << " " << teste_geracao << " " << primeiroMakespan << " " << solucaoInicial << " " << tempoBuscas[0]/execucoesBuscas[0] << " " << tempoBuscas[1]/execucoesBuscas[1] << " " << tempoBuscas[2]/execucoesBuscas[2] << " " << mediaMelhorias[0]/melhoriasBuscas[0] << " " << mediaMelhorias[1]/melhoriasBuscas[1] << " " << mediaMelhorias[2]/melhoriasBuscas[2] << endl;
    cout << "Parametros do Algoritmo\n";
  	cout << "Tamanho da população: " << p << "\n";
  	cout << "Fração da população para elite: " << pe << "\n";
  	cout << "Fração da população trocada por mutantes: " << pm << "\n";
  	cout << "Probabilidade de se herdar um alelo da elite: " << rhoe << "\n";
  	cout << "Populações independentes: " << K << "\n";
  	cout << "Threads para decodificacao paralela: " << MAXT << "\n";
  	cout << "Quantidade máxima de gerações: " << MAX_GENS << "\n";

  	cout << "\n\nParâmetros da instância\n";
  	cout << "Tarefas: " << n << "\n";
  	cout << "Máquinas: " << m << "\n";
  	cout << "Ferramentas: " << t << "\n";
  	cout << "Capacidade Magazine: " << c << "\n";
  	cout << "Tempo de troca: " << tempoTroca << "\n";


    cout << "\n\nMakespan: " << makespan << "\n\n";
    cout << "Tempo de execução: " << time_span.count() << " segundos \n\n";
    cout <<endl<< "Melhor geração: " << teste_geracao << endl;

    cout << "\nSolução inicial: " << primeiroMakespan;
    cout << "\nMakespan ao final da 1ª geração: " << solucaoInicial << "\n\n";

    cout <<"\nBuscas Locais\n";
    cout << "IBS" << endl;
    cout << "Tempo médio: " << tempoBuscas[0]/execucoesBuscas[0] << endl;
    cout << "Número melhorias: " << melhoriasBuscas[0] << endl;
    cout << "Melhora média: " << mediaMelhorias[0]/melhoriasBuscas[0] << endl;
    cout << "Execuções: " << execucoesBuscas[0]<<endl;

    cout << "EFB" << endl;
    cout << "Tempo médio: " << tempoBuscas[1]/execucoesBuscas[1] << endl;
    cout << "Número melhorias: " << melhoriasBuscas[1] << endl;
    cout << "Melhora média: " << mediaMelhorias[1]/melhoriasBuscas[1] << endl;
    cout << "Execuções: " << execucoesBuscas[1]<<endl;

    cout << "ONB" << endl;
    cout << "Tempo médio: " << tempoBuscas[2]/execucoesBuscas[2] << endl;
    cout << "Número melhorias: " << melhoriasBuscas[2] << endl;
    cout << "Melhora média: " << mediaMelhorias[2]/melhoriasBuscas[2] << endl;
    cout << "Execuções: " << execucoesBuscas[2]<<endl;

    cout << "\nMelhor solução" << std::endl;
    maquina = 0;
    for (std::vector<std::vector<int>>::const_iterator i=maquinas.begin(); i!=maquinas.end(); ++i){
      std::vector<int>maquinaAux = *i;
      std::cout << "Máquina " << ++maquina << ": ";
      for (unsigned j=0;j<maquinaAux.size();++j)
        std::cout << maquinaAux[j] << " ";
      std::cout << std::endl;
      std::cout << "Trocas: ";
      std::cout << KTNS(maquinaAux) <<std::endl;
      std::cout << "Tempo de processamento: " << completionTime(tProcessamento,maquinaAux) << "\n";
    }
    std::cout << "Total de gerações: " << generation << "\n";
    // ******************************
	return 0;
}
