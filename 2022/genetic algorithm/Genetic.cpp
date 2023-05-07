#include"Genetic.h"
GENE::GENE() {
}
GENE::GENE(int a, int b, int c, int d) {
	ONEORZERO[0] = a;
	ONEORZERO[1] = b;
	ONEORZERO[2] = c;
	ONEORZERO[3] = d;
}
void GENE::Change(int n, int value) {
	ONEORZERO[n] = value;
}
int GENE::Score() {
	return ONEORZERO[0] + ONEORZERO[1] + ONEORZERO[2] + ONEORZERO[3];
}
int GENE::Get(int n) {
	return ONEORZERO[n];
}
void GENE::Print() {
	cout << "[" << ONEORZERO[0] << ", " << ONEORZERO[1] << ", " << ONEORZERO[2] << ", " << ONEORZERO[3] << "]";
}

Generator::Generator(GENE* a[]) :generation(0){
	for (int i = 0; i < 10; i++) {
		gene[i] = a[i];
	}
}
void Generator::Sort() {
	int Mnum = 0; //if 4 break;
	for (int i = 4; i >= 0; i--) {
		for (int j = 0; j < 10; j++) {
			if (gene[j]->Score() == i) {
				Mgene[Mnum] = gene[j];
				Mnum++;
				if (Mnum == 4) {
					return;
				}
			}
		}
	}
}
GENE* Generator::CrossOver(GENE& a, GENE& b) {
	GENE* newG = new GENE();
	for (int i = 0; i < 4; i++) {
		if (a.Get(i) == b.Get(i))
			newG->Change(i, a.Get(i));
		else {
			if (rand() % 2 == 0) {
				newG->Change(i, a.Get(i));
			}
			else {
				newG->Change(i, b.Get(i));
			}
		}
	}
	return newG;
}
void Generator::Mutation() {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 4; j++) {
			if (rand() % 100 < 3) {
				gene[i]->Change(j, (gene[i]->Get(j) - 1) * (-1));
			}
		}
	}
}
void Generator::Change() {
	Sort();
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (i == 3 && j == 2) {
				break;
			}
			next[i * 4 + j] = CrossOver(*Mgene[i], *Mgene[j]);
		}
	}
	//
	DEL();
	for (int i = 0; i < 10; i++) {
		gene[i] = next[i];
	}
	Mutation();
	generation += 1;
	cout << "¼¼´ë: " << generation << endl;
	for (int i = 0; i < 10; i++) {
		gene[i]->Print();
		cout << endl;
	}
}
void Generator::DEL() {
	for (int i = 0; i < 10; i++) {
		delete gene[i];
	}
}
Generator::~Generator() {
	DEL();
}