# Targets:
all:
		g++ -O3 -fopenmp -march=native -std=c++11 -o "Population" Population.h
		g++ -O3 -fopenmp -march=native -std=c++11 -o "BRKGA" BRKGA.h
		g++ -O3 -fopenmp -march=native -std=c++11 -o "KTNS" KTNS.h
		g++ -O3 -fopenmp -march=native -std=c++11 -o "MTRand" MTRand.h
	 	g++ -O3 -fopenmp -march=native -std=c++11 -o "delta_avaliacao" delta_avaliacao.h
		g++ -O3 -fopenmp -march=native -std=c++11 -o "Buscas" Buscas.h
		g++ -O3 -fopenmp -march=native -std=c++11 -o "SampleDecoder" SampleDecoder.h
		g++ -O3 -fopenmp -march=native -std=c++11 -o "samplecode" samplecode.cpp
		g++ -O3 -fopenmp -march=native -std=c++11 -o "run" run.cpp
