#ifndef __GENETIC_H__
#define __GENETIC_H__
#include<iostream>
#include<random>
using namespace std;

class GENE {
	int ONEORZERO[4];
public:
	GENE();
	GENE(int a, int b, int c, int d);
	void Change(int n, int value);
	int Score();
	int Get(int n);
	void Print();
};

class Generator {
private:
	GENE* gene[10];
	GENE* next[10];
	GENE* Mgene[4];
	int generation;
public:
	Generator(GENE* a[]);
	void Sort();
	GENE* CrossOver(GENE& a, GENE& b);
	void Mutation();
	void Change();
	~Generator();
	void DEL();
};

#endif // !__GENETIC_H__
