#include "test/test.h"
#include <stdio.h>


int main() {
	printf(BLU "\n\nmade by German Dagil P3233 on 06.01.2022\n");
	printf(MAG "---------------------------------------------------\n");
	printf("               SERIES OF TESTS STARTED");
	printf("\n---------------------------------------------------\n\n");
	int counter = 0;
	int tests_amount = 5;
	if (test1()) {
		counter++;
	}
	if (test2()) {
		counter++;
	}
	if (test3()) {
		counter++;
	}
	if (test4()) {
		counter++;
	}
	if (test5()) {
		counter++;
	}
	if (counter == tests_amount) {
		printf(MAG "\n -[   "GRN"ALL TESTS"MAG " WERE SUCCESSFUL   ]-\n");
	}
	else {
		if (counter == 0) {
			printf(MAG "\n -[   "RED"NO TESTS"MAG " WERE SUCCESSFUL   ]-\n");
		}
		else {
			printf(MAG "\n -[   "RED"NOT ALL TESTS"MAG " WERE SUCCESSFUL   ]-\n");
		}
	}
}