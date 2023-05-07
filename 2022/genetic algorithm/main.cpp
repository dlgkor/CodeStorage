#include"Genetic.h"

int main() {
	srand(time(NULL));

	GENE *arr[10];
	arr[0] = new GENE(1, 0, 1, 0);
	arr[1] = new GENE(0, 1, 0, 0);
	arr[2] = new GENE(0, 0, 1, 0);
	arr[3] = new GENE(1, 1, 0, 1);
	arr[4] = new GENE(0, 0, 0, 0);
	arr[5] = new GENE(0, 0, 0, 0);
	arr[6] = new GENE(1, 0, 0, 0);
	arr[7] = new GENE(1, 0, 0, 1);
	arr[8] = new GENE(0, 0, 1, 1);
	arr[9] = new GENE(0, 0, 0, 1);

	Generator GEN(arr);

	for (int i = 0; i < 100; i++) {
		GEN.Change();
	}
}
