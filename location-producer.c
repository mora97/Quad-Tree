#include <stdio.h>

static void min_pass(FILE *file, long ldx, long ldy, long rux, long ruy, long t1,long t2) 
{
	int i;
	long tempX = 0;
	long tempY = -100;
	for (i = 0; i < 1000; i++) {
		ldx = tempX;
		ldy = tempY;
		rux = tempX + 15;
		ruy = tempY + 5;
		tempY = ruy + 5;
		tempX = rux + 5;
		
		fprintf(file, "%ld %ld %ld %ld %ld %ld\n", ldx, ldy, rux, ruy, 5, 10); /* two last variable could be changed*/	
	}
}

static void max_pass(FILE *file, long ldx, long ldy, long rux, long ruy, long t1,long t2) 
{
	int i;
	long tempX = -20000;
	long tempY = -1000;
	for (i = 0; i < 1000; i++) {
		ldx = tempX;
		ldy = tempY;
		rux = tempX + 90;
		ruy = tempY + 70;
		tempY = ruy + 5;
		tempX = rux + 5;
		
		fprintf(file, "%ld %ld %ld %ld %ld %ld\n", ldx, ldy, rux, ruy, i + 10, i + 15); /* two last variable could be changed*/
		
		if (tempY > 1000) {
			tempY = -1000;
		}
		
		if (tempX > 0) {
			tempX = -20000;
		}		
	}
}

int main()
{
	long ldx, ldy, rux, ruy, t1, t2;
	int state = -1;
	FILE *wf;
	
	printf("Enter state(0: minimum passed, 1: maximum passed): ");
	scanf("%d", &state);
	wf = fopen("test-cases/test.txt", "w");
	if (!wf) {
		printf("no file"); 
	}
	
	
	if (state == 0) {
		min_pass(wf, ldx, ldy, rux, ruy, t1, t2);
	} else {
		max_pass(wf, ldx, ldy, rux, ruy, t1, t2);
	}
	
}
