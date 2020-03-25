#ifndef DELTA_AVALICAO_H
#define DELTA_AVALICAO_H
#include <bitset>
#include <vector>
#include <iostream>
using namespace std;

#define MAXBIT 40

extern vector < bitset <MAXBIT> > bitMatrix;
extern unsigned n; // tarefas
int deltaBitwise(int a, int b, int c){
	return (~bitMatrix[a]&bitMatrix[b]&~bitMatrix[c]|bitMatrix[a]&~bitMatrix[b]&bitMatrix[c]).count();
}

int deltaShift (int i, int j){
  return -deltaBitwise(i-1, i, i+1) + deltaBitwise(j-1, i, j);
}
int deltinha (int i, int j, int k){
  // inserir k entre i e j
  // cout << "Tarefas: " << i << " " << j << " " << k << " \n";
  extern std::vector<std::vector<int>> matrixFerramentas;
  int oneBlocks = 0;
  for (int linha = 0; linha < matrixFerramentas.size(); ++linha){
    // if (j==-1)
    //   cout << matrixFerramentas[linha][i] << matrixFerramentas[linha][k] << "0J\n";
    // else if (i==-1)
    //   cout << "i0" << matrixFerramentas[linha][k] << matrixFerramentas[linha][j] << "\n";
    // else
    //   cout << matrixFerramentas[linha][i] << matrixFerramentas[linha][k] << matrixFerramentas[linha][j] << "\n";
    if (j==-1){
      // ta inserindo após a última, j é 0
      if (matrixFerramentas[linha][i]==0 && matrixFerramentas[linha][k]==1){
        ++oneBlocks;
        continue;
      }
    }
    if (i==-1){
      // ta inserindo antes da primeira
      if (matrixFerramentas[linha][j]==0 && matrixFerramentas[linha][k]==1){
        ++oneBlocks;
        continue;
      }
    }
    if (matrixFerramentas[linha][i]==0 && matrixFerramentas[linha][j]==0 && matrixFerramentas[linha][k]==1){
      ++oneBlocks;
      continue;
    }
    if (matrixFerramentas[linha][i]==1 && matrixFerramentas[linha][j]==1 && matrixFerramentas[linha][k]==0) {
      ++oneBlocks;
    }
  }
  return oneBlocks;
}

#endif
