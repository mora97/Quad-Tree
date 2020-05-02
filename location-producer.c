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
		
		fprintf(file, "%ld %ld %ld %ld %ld %ld\n", ldx, ldy, rux, ruy, 10, 15); /* two last variable could be changed*/	
	}
}

static void max_pass(FILE *file, long ldx, long ldy, long rux, long ruy, long t1,long t2) 
{
	int i;
	long tempX = -30000;
	long tempY = -4000;
	for (i = 0; i < 1000; i++) {
		ldx = tempX;
		ldy = tempY;
		rux = tempX + 320;
		ruy = tempY + 290;
		tempY = ruy + 5;
		tempX = rux + 5;
		
		fprintf(file, "%ld %ld %ld %ld %ld %ld\n", ldx, ldy, rux, ruy, 0, 1000000); /* two last variable could be changed*/
		
		if (tempY > 0) {
			tempY = -4000;	
		}
		
		if (tempX > -100) {
			tempX = -30000;
		}		
	}
}

static void max_pass_temp(FILE *file, long ldx, long ldy, long rux, long ruy, long t1,long t2) 
{
	int i;
	long tempX = -10000;
	long tempY = 0;
	for (i = 0; i < 1000; i++) {
		ldx = tempX;
		ldy = tempY;
		rux = tempX + 90;
		ruy = tempY + 70;
		tempY = ruy + 5;
		tempX = rux + 5;
		
		fprintf(file, "%ld %ld %ld %ld %ld %ld\n", ldx, ldy, rux, ruy, 0, 1000000); /* two last variable could be changed*/
		
		if (tempY > 1500) {
			tempY = 0;	
		}
		
		if (tempX > 100) {
			tempX = -10000;
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
	wf = fopen("test-cases/min_passes-time.txt", "w");
	if (!wf) {
		printf("no file"); 
	}
	fprintf(wf, "%ld\n",1000); /* two last variable could be changed*/
		
	if (state == 0) {
		min_pass(wf, ldx, ldy, rux, ruy, t1, t2);
	} else if (state == 1) {
		max_pass(wf, ldx, ldy, rux, ruy, t1, t2);
	} else if (state == 2) {
		max_pass_temp(wf, ldx, ldy, rux, ruy, t1, t2);
	}
}
