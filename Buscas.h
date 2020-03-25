#ifndef BUSCAS_H
#define BUSCAS_H

#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <string.h>
#include <bitset>
#include <random>
#include <ctime>
#include <ratio>
#include <string>


#include "KTNS.h"
#include "delta_avaliacao.h"
#define MAXBIT 40
extern std::vector < std::bitset <MAXBIT> > bitMatrix;
extern int m;
extern std::vector<unsigned>tProcessamento;
extern double tempoBuscas[3]; // IBNS, EFB, ONB
extern long execucoesBuscas[3];
extern long melhoriasBuscas[3];
extern double mediaMelhorias[3];
std::chrono::high_resolution_clock::time_point tb1;
std::chrono::high_resolution_clock::time_point tb2;
std::chrono::duration<double> time_span_b;
double IBSMakespan, EFBMakespan, ONBMakespan;
using namespace std;

void IBS(std::vector < std::vector <int> >& maquinas, std::vector < std::pair <double,int> >& idx_maquinas, double& makespan, std::vector<unsigned> tProcessamento){
    // maquinas - Todas as Máquinas
    // idx_maquinas - completionTime, idMaquina
    // makespan
    // tProcessamento - tempo de processamento das tarefas (carregado na decodificacao)
    std::vector<std::pair<double, int>>::iterator critica = idx_maquinas.begin();
    std::vector<std::pair<double, int>>::iterator best = idx_maquinas.end() -1;
    bool melhorou = true;
    double c1 = 0;
    double c2 = 0;
    double c3 = 0;
    double c4 = makespan;
    if (idx_maquinas.size() < m)
      melhorou = false;
    std::sort(idx_maquinas.rbegin(), idx_maquinas.rend());
    while (melhorou) {
      int melhorPosicao = 0;
      critica = idx_maquinas.begin();
      best = idx_maquinas.end() -1;
      std::vector<int> mCritica = maquinas.at(critica->second -1);
      int lastTask = mCritica[mCritica.size() -1];
      // se o tempo de processamento da tarefa + completion time já excede o makespan não ha porque continuar
      if (best->first + tProcessamento[lastTask]>=makespan)
        return;

      mCritica.pop_back();
      c1 = completionTime(tProcessamento, mCritica);
      std::vector<int> mBest = maquinas.at(best->second -1);
      int tMbest = mBest.size()-1;

      mBest.push_back(lastTask);
      c2 = completionTime(tProcessamento, mBest);

      melhorPosicao = tMbest+1;
      for (int i=tMbest+1;i>0;--i){
          mBest[i]=mBest[i-1];
          mBest[i-1] = lastTask;
          c3 = completionTime(tProcessamento, mBest);
          if (c3<c2){
            melhorPosicao = i-1;
            c2 = c3;
          }
      }
      c3 = max(c1,c2);
      if (c3<makespan){
        // a tarefa incluída estará em zero. Movê-la para a melhor posição
        if (melhorPosicao!=0){
            for (int i =1; i<=melhorPosicao; ++i )
                mBest[i-1]=mBest[i];
            mBest[melhorPosicao] = lastTask;
        }
        critica->first = c1;
        best->first = c2;
        maquinas.at(critica->second -1)=mCritica;
        maquinas.at(best->second -1)=mBest;
        std::sort(idx_maquinas.rbegin(), idx_maquinas.rend());
        critica = idx_maquinas.begin();
        makespan = critica->first;
      } else
        melhorou=false;
    }
}
void EFB(std::vector < std::vector <int> >& maquinas, std::vector < std::pair <double,int> >& idx_maquinas, double& makespan, std::vector<unsigned> tProcessamento){
    // maquinas - Todas as Máquinas
    // idx_maquinas - completionTime, idMaquina
    // makespan
    // tProcessamento - tempo de processamento das tarefas (carregado na decodificacao)
    extern std::vector<std::vector<int>> matrixFerramentas;
  	extern unsigned t; // ferramentas
    extern int m; // máquinas
    int machineTools[t][m];
    std::vector<std::pair<double, int>>::iterator critica;
    std::vector<std::pair<double, int>>::iterator best;
    memset(machineTools, 0, sizeof(machineTools[0][0]) * t * m);
    if (idx_maquinas.size() < m)
      return ;

    for (int k=0; k<t; ++k)
      for (int i=0; i<m; ++i){
        std::vector<int> maquina = maquinas.at(i);
        for (std::vector<int>::const_iterator j=maquina.begin(); j!=maquina.end();++j){
          for (int k=0; k<t; ++k)
          machineTools[k][i]+=matrixFerramentas[k][*j];
        }
      }
    std::sort(idx_maquinas.rbegin(), idx_maquinas.rend());
    bool melhorou = true;
    bool corrija = false;
    while(melhorou){
        melhorou = false;

        bool b1 =false;
        bool b2=false;
        int b1T = 0; // Quantidade de ferramentas utilizadas pela tarefa
        int b2T = 0;
        int b1C = 0; // Quantidade de ferramentas compatíveis
        int b2C = 0;
        critica = idx_maquinas.begin();
        best = idx_maquinas.end() -1;
        std::vector<int> mCritica = maquinas.at(critica->second -1);
        std::vector<int> mBest = maquinas.at(best->second -1);
        for (int i=0; i < mCritica.size(); ++i){
          int tCritica = mCritica[i];
          // Atualiza ferramentas da máquina Critica
          for (int k=0; k < t; ++k){
                machineTools[k][critica->second -1]-=matrixFerramentas[k][tCritica];
          }
          for (int j=0; j < mBest.size(); ++j){
            int tBest = mBest[j];
            // Atualiza ferramentas da máquina Best
            for (int k=0; k < t; ++k){
                  machineTools[k][best->second -1]-=matrixFerramentas[k][tBest];
            }
            // Confere compatibilidade
            for (int k=0; k < t; ++k){
              if (matrixFerramentas[k][tCritica]==1){
                  b1T++;
                  if (machineTools[k][critica->second -1]>=1)
                    b1C++;
              }
              if (matrixFerramentas[k][tBest]==1){
                  b2T++;
                  if (machineTools[k][best->second -1]>=1)
                    b2C++;
              }
            }
            if (b1T <= (b1C*2))
              b1 = true;
            if (b2T <= (b2C*2))
              b2 = true;
            //   int delt;
            // if (i==0)
            //   delt = deltinha(-1,mCritica[i+1],tCritica);
            // else if (i==mCritica.size()-1)
            //   delt = deltinha(mCritica[i-1],-1,tCritica);
            // else
            //   delt = deltinha(mCritica[i-1],mCritica[i+1],tCritica);
            if ((b1) && (b2) ){
              // std::cout << "Podemos trocar " << tCritica << " e " << tBest;
              mBest[j]=tCritica;
              mCritica[i]=tBest;
              double c1 = completionTime(tProcessamento, mCritica);
              if (c1<makespan){
                double c2 = completionTime(tProcessamento, mBest);
                double c3 = max(c1,c2);
                if (c3<makespan){
                //  std::cout << " Troquei, passou de " << makespan << " para " << c3;
                  critica->first = c1;
                  best->first = c2;
                  maquinas.at(critica->second -1)=mCritica;
                  maquinas.at(best->second -1)=mBest;
                  std::sort(idx_maquinas.rbegin(), idx_maquinas.rend());
                  makespan = critica->first;
                  // Corrrigir o cromossomo
                  corrija = true;
                  melhorou = true;
                  break;
                }
              }
              if (!melhorou) {
                // Desfazer a troca
                // std::cout << " Troquei mas não melhorou, desfiz ";
                mBest[j]=tBest;
                mCritica[i]=tCritica;
                // Devolve ferramentas da máquina Best
                for (int k=0; k < t; ++k){
                      machineTools[k][best->second -1]+=matrixFerramentas[k][tBest];
                }
              }
            } else{ // se não ha compatibilidade
              // Devolve ferramentas da máquina Best
              for (int k=0; k < t; ++k){
                    machineTools[k][best->second -1]+=matrixFerramentas[k][tBest];
              }
              continue;
            }
          }
          if (melhorou)
            break;
          else {
            // Devolve as ferramentas para Critica'
            for (int k=0; k < t; ++k){
                  machineTools[k][critica->second -1]+=matrixFerramentas[k][tCritica];
            }
          }
        }// Para tarefas criticas
    } // wend
}
void ONB(std::vector < std::vector <int> >& maquinas, std::vector < std::pair <double,int> >& idx_maquinas, double& makespan, std::vector<unsigned> tProcessamento){
    // maquinas - Todas as Máquinas
    // idx_maquinas - completionTime, idMaquina
    // makespan
    // tProcessamento - tempo de processamento das tarefas (carregado na decodificacao)
    extern std::vector<std::vector<int>> matrixFerramentas;
  	extern unsigned t; // ferramentas
    extern unsigned n; // tarefas
    std::vector<std::pair<double, int>>::iterator critica;
    std::pair<int, int>ONB1, ONB2;
    int deltad, deltae;
    if (idx_maquinas.size() < m)
      return ;
    bool melhorou = true;
    double cTimeCritica = 0;
    while (melhorou){
        melhorou = false;
        std::sort(idx_maquinas.rbegin(), idx_maquinas.rend());
        critica = idx_maquinas.begin();
        cTimeCritica = critica->first;
        makespan = cTimeCritica;
        std::vector<int> mCritica = maquinas.at(critica->second -1);
        std::vector<int> mAux1,mAux2;
        if (KTNS(mCritica)==0)
            return ;
        for (unsigned i =0; i<=n+2;++i) bitMatrix[i].reset();

    	  for (unsigned j = 0; j<t; ++j){
    		    for (unsigned i = 0; i<mCritica.size(); ++i){
    			       if
                 (matrixFerramentas[j][mCritica[i]]==1){
    				           bitMatrix[i+1].set(t-1-j);
                  }
    		    }
        }
        /*
        int ferramentas[t][mCritica.size()];
        for (unsigned j=0; j<t; ++j) {
      		for (unsigned i =0; i<mCritica.size(); ++i) {
            ferramentas[j][i] = matrixFerramentas[j][mCritica[i]];
          }
        }
        */
        std::vector<int> linhas;
        for(int i = 0; i<t; ++i)
          linhas.push_back(i);
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        shuffle (linhas.begin(), linhas.end(), std::default_random_engine(seed));

        for (vector<int>::const_iterator i = linhas.begin(); i!= linhas.end(); ++i){
          ONB1 = std::make_pair(-1,-1);
          ONB2 = std::make_pair(-1,-1);
          for (unsigned j=0; j<mCritica.size();++j){
            if (matrixFerramentas[*i][mCritica[j]]==1){
              if (ONB1.first == -1){
                ONB1.first = j;
                while (j < mCritica.size() && matrixFerramentas[*i][mCritica[j]]==1) ++j;
                ONB1.second = j-1;
              } else {
                if (ONB2.first == -1){
                  ONB2.first = j;
                  while (j < mCritica.size() && matrixFerramentas[*i][mCritica[j]]==1) ++j;
                  ONB2.second = j-1;
                }
              }
              if (ONB2.first!=-1){
                int nMovimentos = ONB1.first - ONB1.second +1;
                int pivo = ONB1.first;
                int TPivo = 0;
                mAux1 = mCritica;
                mAux2 = mCritica;
                double c1,c2;
                for (int p=0;p<nMovimentos;++p){
                  //delta avalicao indice la começa em 1
                  deltae = deltaShift(pivo+1,ONB2.first+1);
                  deltad = deltaShift(pivo+1,ONB2.second+2);
                  if (deltae<=0 || deltad <=0){
                    // Insiro a esquerda do 2 ONB
                    TPivo=mAux1[pivo];
                    for (int pe=pivo;pe<ONB2.first-1;++pe)
                      mAux1[pe]=mAux1[pe+1];
                    mAux1[ONB2.first-1] = TPivo;
                    c1 = completionTime(tProcessamento,mAux1);

                    //std::cout << "\nDelta Esquerda " << deltae << "\n";
                    // Insiro a direita do 2 ONB
                    TPivo=mAux2[pivo];
                    for (int pd=pivo;pd<ONB2.second;++pd)
                      mAux2[pd]=mAux2[pd+1];
                    mAux2[ONB2.second] = TPivo;
                    c2 = completionTime(tProcessamento,mAux2);

                    //std::cout << "\nDelta Direita " << deltad << "\n";
                    if (c1 < cTimeCritica || c2 < cTimeCritica){
                      melhorou = true;
                      /*
                      std::cout << "Melhorou" << "\n deltaE: " << deltae << " deltaD: " << deltad << "\n";
                      if (deltae > 0 && deltad > 0) {
                        std::cout << "wtf" << '\n';
                        for (unsigned i = 0; i<mAux2.size()+2; ++i){
                              std::cout << bitMatrix[i] << '\n';
                        }
                        cout << pivo+1 << " para " << ONB2.first+1 << "\n";
                        cout << pivo+1 << " para " << ONB2.second+2 << "\n";

                      }
                      // *****************************************/
                      if (c1<c2){
                        // Fica à esquerda
                      //  std::cout << "\nFicou a esquerda\n";
                        mCritica = mAux1;
                        ONB2.first = ONB2.first -1;
                        cTimeCritica = c1;
                      } else {
                        // Fica à direita
                      //  std::cout << "\nFicou a direita\n";
                        mCritica = mAux2;
                        cTimeCritica = c2;
                      }
                    }
                  } // fim da delta avaliacao <= 0
                  if (!melhorou)
                    ++pivo; // se tiver melhorado, a tarefa andou, então o pivo fica.
                  mAux1=mCritica;
                  mAux2=mCritica;
                } // Fim dos movimentos ONB1->ONB2
                // Procura-se o proximo ONB
                ONB1.first = ONB2.first;
                ONB1.second = ONB2.second;
                ONB2 = make_pair(-1,-1);
              }
            }
          }
        } // fim das linhas
        if (melhorou){
          critica->first = cTimeCritica;
          maquinas.at(critica->second -1) = mCritica;
        }

    } // wend

}
void ONB_m(std::vector <int>& mCritica, double& cTimeCritica,std::vector<unsigned> tProcessamento){
    // mCritica - máquina
    // idx_maquinas - completionTime, idMaquina
    // cTimeCritica - completionTime da máquina
    // tProcessamento - tempo de processamento das tarefas (carregado na decodificacao)
    extern std::vector<std::vector<int>> matrixFerramentas;
  	extern unsigned t; // ferramentas
    std::pair<int, int>ONB1, ONB2;
    bool melhorou = true;
    while (melhorou){
        melhorou = false;
        std::vector<int> mAux1,mAux2;
        if (KTNS(mCritica)==0)
            return ;

        std::vector<int> linhas;
        for(int i = 0; i<t; ++i)
          linhas.push_back(i);
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        shuffle (linhas.begin(), linhas.end(), std::default_random_engine(seed));

        for (vector<int>::const_iterator i = linhas.begin(); i!= linhas.end(); ++i){
          ONB1 = std::make_pair(-1,-1);
          ONB2 = std::make_pair(-1,-1);
          for (unsigned j=0; j<mCritica.size();++j){
            if (matrixFerramentas[*i][mCritica[j]]==1){
              if (ONB1.first == -1){
                ONB1.first = j;
                while (j < mCritica.size() && matrixFerramentas[*i][mCritica[j]]==1) ++j;
                ONB1.second = j-1;
              } else {
                if (ONB2.first == -1){
                  ONB2.first = j;
                  while (j < mCritica.size() && matrixFerramentas[*i][mCritica[j]]==1) ++j;
                  ONB2.second = j-1;
                }
              }
              if (ONB2.first!=-1){
                int nMovimentos = ONB1.first - ONB1.second +1;
                int pivo = ONB1.first;
                int TPivo = 0;
                mAux1 = mCritica;
                mAux2 = mCritica;
                double c1,c2;
                for (int p=0;p<nMovimentos;++p){
                  // Insiro a esquerda do 2 ONB
                  TPivo=mAux1[pivo];
                  for (int pe=pivo;pe<ONB2.first;++pe)
                    mAux1[pe]=mAux1[pe+1];
                  mAux1[ONB2.first] = TPivo;
                  c1 = completionTime(tProcessamento,mAux1);
                  // Insiro a direita do 2 ONB
                  TPivo=mAux2[pivo];
                  for (int pd=pivo;pd<ONB2.second;++pd)
                    mAux2[pd]=mAux2[pd+1];
                  mAux2[ONB2.second] = TPivo;
                  c2 = completionTime(tProcessamento,mAux2);
                  if (c1 < cTimeCritica || c2 < cTimeCritica){
                    melhorou = true;
                    if (c1<c2){
                      // Fica à esquerda
                      mCritica = mAux1;
                      ONB2.first = ONB2.first -1;
                      cTimeCritica = c1;

                    } else {
                      // Fica à direita
                      mCritica = mAux2;
                      cTimeCritica = c2;
                    }
                  }
                  ++pivo;
                  mAux1=mCritica;
                  mAux2=mCritica;
                } // Fim dos movimentos ONB1->ONB2
                // Procura-se o proximo ONB
                ONB1.first = ONB2.first;
                ONB1.second = ONB2.second;
                ONB2 = make_pair(-1,-1);
              }
            }
          }
        } // fim das linhas
    } // wend

}
double buscas(std::vector< double >& chromosome, int vezes = 1){
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
  // vezes -1 = VND
  if (vezes == -1 ){
    int vnd = 1;
    while (vnd<4){
      if (vnd == 1){
        tb1 = std::chrono::high_resolution_clock::now();
        IBSMakespan = makespan;
        IBS(maquinas,idx_maquinas,makespan,tProcessamento);
        if (IBSMakespan>makespan) {
            vnd = 1;
            ++melhoriasBuscas[0];
            mediaMelhorias[0]+= (IBSMakespan-makespan);
        } else
          ++vnd;
        tb2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration_cast<std::chrono::duration<double>>(tb2 - tb1);
      	time_span_b = std::chrono::duration_cast<std::chrono::duration<double>>(tb2 - tb1);
        tempoBuscas[0]+=time_span_b.count();
        execucoesBuscas[0]++;

      }
      if (vnd == 2){
        tb1 = std::chrono::high_resolution_clock::now();
        EFBMakespan = makespan;
        EFB(maquinas,idx_maquinas,makespan,tProcessamento);
        if (EFBMakespan>makespan) {
          vnd = 1;
          ++melhoriasBuscas[1];
          mediaMelhorias[1]+= (EFBMakespan-makespan);
        } else
          ++vnd;
        tb2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration_cast<std::chrono::duration<double>>(tb2 - tb1);
        time_span_b = std::chrono::duration_cast<std::chrono::duration<double>>(tb2 - tb1);
        tempoBuscas[1]+=time_span_b.count();
        execucoesBuscas[1]++;

      }
      if (vnd == 3){
        tb1 = std::chrono::high_resolution_clock::now();
        ONBMakespan = makespan;
        ONB(maquinas,idx_maquinas,makespan,tProcessamento);
        if (ONBMakespan>makespan){
          vnd = 1;
          ++melhoriasBuscas[2];
          mediaMelhorias[2]+= (ONBMakespan-makespan);
        } else
          ++vnd;
        tb2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration_cast<std::chrono::duration<double>>(tb2 - tb1);
      	time_span_b = std::chrono::duration_cast<std::chrono::duration<double>>(tb2 - tb1);
        tempoBuscas[2]+=time_span_b.count();
        execucoesBuscas[2]++;

      }
    }
  } else {  // Se não for aplicar o VND
    for(int i = 0; i < vezes; ++i){
      tb1 = std::chrono::high_resolution_clock::now();
      IBSMakespan = makespan;
      IBS(maquinas,idx_maquinas,makespan,tProcessamento);
      if (IBSMakespan>makespan) {
          ++melhoriasBuscas[0];
          mediaMelhorias[0]+= (IBSMakespan-makespan);
      }
      tb2 = std::chrono::high_resolution_clock::now();
      std::chrono::duration_cast<std::chrono::duration<double>>(tb2 - tb1);
    	time_span_b = std::chrono::duration_cast<std::chrono::duration<double>>(tb2 - tb1);
      tempoBuscas[0]+=time_span_b.count();
      execucoesBuscas[0]++;

      tb1 = std::chrono::high_resolution_clock::now();
      EFBMakespan = makespan;
      EFB(maquinas,idx_maquinas,makespan,tProcessamento);
      if (EFBMakespan>makespan) {
        ++melhoriasBuscas[1];
        mediaMelhorias[1]+= (EFBMakespan-makespan);
      }
      tb2 = std::chrono::high_resolution_clock::now();
      std::chrono::duration_cast<std::chrono::duration<double>>(tb2 - tb1);
    	time_span_b = std::chrono::duration_cast<std::chrono::duration<double>>(tb2 - tb1);
      tempoBuscas[1]+=time_span_b.count();
      execucoesBuscas[1]++;

      tb1 = std::chrono::high_resolution_clock::now();
      ONBMakespan = makespan;
      ONB(maquinas,idx_maquinas,makespan,tProcessamento);
      if (ONBMakespan>makespan){
        ++melhoriasBuscas[2];
        mediaMelhorias[2]+= (ONBMakespan-makespan);
      }
      tb2 = std::chrono::high_resolution_clock::now();
      std::chrono::duration_cast<std::chrono::duration<double>>(tb2 - tb1);
    	time_span_b = std::chrono::duration_cast<std::chrono::duration<double>>(tb2 - tb1);
      tempoBuscas[2]+=time_span_b.count();
      execucoesBuscas[2]++;

    }
  }

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

#endif
