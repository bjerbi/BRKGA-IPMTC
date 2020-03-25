/*
 * SampleDecoder.h
 *
 * Any decoder must have the format below, i.e., implement the method decode(std::vector< double >&)
 * returning a double corresponding to the fitness of that vector. If parallel decoding is to be
 * used in the BRKGA framework, then the decode() method _must_ be thread-safe; the best way to
 * guarantee this is by adding 'const' to the end of decode() so that the property will be checked
 * at compile time.
 *
 * The chromosome inside the BRKGA framework can be changed if desired. To do so, just use the
 * first signature of decode() which allows for modification. Please use double values in the
 * interval [0,1) when updating, thus obeying the BRKGA guidelines.
 *
 *  Created on: Jan 14, 2011
 *      Author: rtoso
 */

#ifndef SAMPLEDECODER_H
#define SAMPLEDECODER_H

#include <list>
#include <vector>
#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>
#include "KTNS.h"
#include "Buscas.h"
class SampleDecoder {
public:
	SampleDecoder()  { }
	SampleDecoder(std::vector<unsigned> _tProcessamento): tProcessamento(_tProcessamento) { }
	~SampleDecoder() { }

	double decode(std::vector< double >& chromosome) const{
		extern int m;
		std::vector < std::vector <int> > maquinas; // Todas as máquinas
		std::vector < std::pair <double,int> > idx_maquinas; // completionTime, machine
		int maquina = 0;
		int at = 0;
		double tPr = 0;
		double makespan = 0.0;
		std::vector < std::pair < double, unsigned > > ranking(chromosome.size());
		for(unsigned i = 0; i < chromosome.size(); ++i){
			ranking[i]=std::pair<double,unsigned>(chromosome[i],i);
		}

		std::sort(ranking.begin(), ranking.end());
		std::vector<int> processos;
		int nTrocas = 0;
		long tTrocas = 0;
		extern unsigned tempoTroca;
		tPr = 0.0;
		for(std::vector<std::pair<double, unsigned>>::const_iterator i = ranking.begin(); i!=ranking.end(); ++i){
			at = (int) i->first; // A parte inteira do alelo representa a máquina
			if (maquina==0){
				maquina = at;
				processos.clear();
			}
			if (maquina==at){
				tPr += tProcessamento[i->second];
				processos.push_back(i->second);
			} else {
				nTrocas=KTNS(processos);
				tTrocas = tempoTroca*nTrocas;

				if ((tPr+tTrocas)>makespan){
					makespan = tPr+tTrocas;
				}

				maquinas.push_back(processos);
				idx_maquinas.push_back(std::pair<double,int>((tPr+tTrocas),maquina));

				maquina = at;
				processos.clear();
				tPr = tProcessamento[i->second];
				processos.push_back(i->second);

			}
		}
		nTrocas=KTNS(processos);
		tTrocas = tempoTroca*nTrocas;
		if ((tPr+tTrocas)>makespan){
			makespan = tPr+tTrocas;
		}
		maquinas.push_back(processos);
		idx_maquinas.push_back(std::pair<double,int>((tPr+tTrocas),maquina));

    double oldMakespan = makespan;
    // std::random_device rd;
    // std::mt19937 gen(rd());
    // std::uniform_real_distribution<> dis(0,1);
    // float moeda = dis(gen);
    // if (moeda>0.95){
    //   IBS(maquinas,idx_maquinas,makespan,tProcessamento);
    //   EFB(maquinas,idx_maquinas,makespan,tProcessamento);
    //   ONB(maquinas,idx_maquinas,makespan,tProcessamento);
    // }

    if (oldMakespan!=makespan){
    //  std::cout << "foi: " << oldMakespan << " voltou "  << makespan << "\n";
      unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
      std::default_random_engine generator(seed);
      int pRanking = 0;
      // Corrijo o  chromosome
      int pMaquina = 0;
      double lInferior = 0;
      for(std::vector<std::vector<int>>::const_iterator i = maquinas.begin(); i!=maquinas.end(); ++i){
        std::vector<int>maquinaAux = *i;
        ++pMaquina;
        // Sortear N numeros, ordenar e distribuir
        std::vector<double> tempKey;
        std::uniform_real_distribution<double> distribution(pMaquina,pMaquina+1);
        for (int b=0;b<maquinaAux.size();++b)
          tempKey.push_back(distribution(generator));
        std::sort(tempKey.begin(), tempKey.end());
        int kIdx = 0;
        for (std::vector<int>::const_iterator j = maquinaAux.begin(); j!=maquinaAux.end(); ++j){
          ranking[pRanking].second = *j;
          ranking[pRanking].first = tempKey[kIdx];
          ++kIdx;
          /*
          if ((ranking[pRanking].first >= pMaquina+1) ||(ranking[pRanking].first < pMaquina) ){
            if (ranking[pRanking-1].first < pMaquina) lInferior = pMaquina; else lInferior = ranking[pRanking-1].first;
            std::uniform_real_distribution<double> distribution(lInferior,pMaquina+1);
            ranking[pRanking].first = distribution(generator);
          }
          */
          chromosome[ranking[pRanking].second] = ranking[pRanking].first;
          ++pRanking;
        }
      }
    }
		return makespan;
		}

private:
	std::vector<unsigned> tProcessamento;
};

#endif
