#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>

using namespace std;

#define CD(folder, command) ("cd " + folder + " && " + command)

int main() {
	string dmakeFolder = "../../../";
	string baseFolder = "examples/makefiles/";
	string mpirun = "mpirun -machinefile " + dmakeFolder + "machines.txt -np ";
	string dmake = dmakeFolder + "dmake";
	string benchmarks[] = {"matrix"};
	int numBenchmarks = 1;
	int numTimes = 1;
	int maxMachines = 20;

	time_t begin, end;
	double tExec = 0;
	double tMake = 0;
	double tDMake1core = 0;
	
	for(int i=0; i<numBenchmarks; i++) {
		FILE* output = fopen("benchmark.dat", "w");
		tExec = 0;
		tMake = 0;
//*
		for(int t=0; t<numTimes; t++) {
			system(CD(baseFolder+benchmarks[i], " make clean").c_str());
			string command = CD(baseFolder+benchmarks[i], " make");
			cout << "Executing '" << command << "'" << endl;
			begin = time(NULL);
			system(command.c_str());
			end = time(NULL);
			tExec += difftime(end, begin);
		}
		tExec /= numTimes;
		tMake = tExec;
//*/

		for(int m=5; m<=maxMachines; m+=5) {
			tExec = 0;
			for(int t=0; t<numTimes; t++) {
				system(CD(baseFolder+benchmarks[i], " make clean").c_str());
				char strNumMachines[10];
				sprintf(strNumMachines, "%d", m);
				string command = CD(baseFolder+benchmarks[i], mpirun) + strNumMachines;
				command += " " + dmake;
				cout << "Executing '" << command << "'" << endl;
				begin = time(NULL);
				system(command.c_str());
				end = time(NULL);
				tExec += difftime(end, begin);
			}
			tExec /= numTimes;
			if(m == 2) tDMake1core = tExec;
			fprintf(output, "%d\t%lf\n", m, tExec);
		}

		fclose(output);
		FILE* plotFile = fopen("plot.txt", "w");
		fprintf(plotFile, "set terminal png;\n");
		fprintf(plotFile, "set output 'benchmark_%s.png'\n", benchmarks[i].c_str());
		fprintf(plotFile, "set xlabel \"Nombre de Machines\";\n");
		fprintf(plotFile, "set ylabel \"Temps\";\n");
		fprintf(plotFile, "set title \"%s\";\n", benchmarks[i].c_str());
		fprintf(plotFile, "plot 'benchmark.dat' using 1:2 with linespoints title \"dmake\", %lf with line title \"make\"\n", tMake);
		fclose(plotFile);
		string command = "gnuplot plot.txt";
		cout << "Executing '" << command << "'" << endl;
		system(command.c_str());
	}

	return 0;
}