#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

#define INF		0x7fffffff
 
 struct pathNode {
	long x, y, t; 
};

struct paths {
	int n;
	struct pathNode *pathNodes;
};

struct locationNode {
	double ldx, ldy, rux, ruy, minT, maxT;
	int id;
};

struct quadNode {
	double x, y, diffX, diffY;
	int n, parent, lbChild, ltChild, rbChild, rtChild;
	int isRoot; 
	struct locationNode location;
};

static void add_quad_node(struct quadNode *quadTree, struct quadNode *qnode, struct locationNode *lnode, int *nNodes) ;


static struct quadNode init_node(int n, int isRoot, double x, double y, double diffX, double diffY, 
						int parent, int lbChild, int ltChild, int rbChild, int rtChild, struct locationNode *location) 
{
	struct quadNode node;
	node.n = n;
	node.isRoot = isRoot;
	node.x = x;
	node.y = y;
	node.diffX = diffX;
	node.diffY = diffY;
	node.parent = parent;
	node.lbChild = lbChild;
	node.ltChild = ltChild;
	node.rbChild = rbChild;
	node.rtChild = rtChild;

	if (location != NULL) {
		node.location = *location;
	}			
	return node;
}

/* qnode1: parent, qnode2: child */
static void update_quad_tree(struct quadNode *quadTree, struct quadNode *qnode1, struct quadNode *qnode2, int *nNodes) 
{
	struct quadNode tnode;
	int flag = 0;
	
	if (qnode2->location.ldx >= qnode1->x && qnode2->location.ldy >= qnode1->y &&
		qnode2->location.rux > qnode1->x && qnode2->location.ruy > qnode1->y) {
		qnode1->rtChild = qnode1->n;
	} else if (qnode2->location.ldx < qnode1->x && qnode2->location.ldy >= qnode1->y &&
			   qnode2->location.rux <= qnode1->x && qnode2->location.ruy > qnode1->y) {
		qnode1->ltChild = qnode1->n;
	} else if (qnode2->location.ldx >= qnode1->x && qnode2->location.ldy < qnode1->y &&
			   qnode2->location.rux > qnode1->x && qnode2->location.ruy <= qnode1->y) {
		qnode1->rbChild = qnode1->n;
	} else if (qnode2->location.ldx < qnode1->x && qnode2->location.ldy < qnode1->y &&
			   qnode2->location.rux <= qnode1->x && qnode2->location.ruy <= qnode1->y) {
		qnode1->lbChild = qnode1->n;
	} else {
		flag = 1;
	} 
	
	qnode1->parent = qnode2->parent;
	tnode = *qnode1;
	tnode.n = qnode2->n;
	qnode2->parent = tnode.n;
	qnode2->n = qnode1->n;
	quadTree[qnode2->n] = *qnode2;
	quadTree[tnode.n] = tnode;
		
	if (flag == 1) {
		add_quad_node(quadTree, qnode2, &qnode1->location, nNodes);	
	}
}

static void add_quad_node(struct quadNode *quadTree, struct quadNode *qnode, struct locationNode *lnode, int *nNodes) 
{
	double x;
	double y;				
	double rbx = qnode->x + qnode->diffX;
	double lbx = qnode->x - qnode->diffX;
	double tby = qnode->y + qnode->diffY;
	double bby = qnode->y - qnode->diffY;

	if (qnode->isRoot && lnode == NULL) {
		quadTree[*nNodes] = *qnode;
	} else {
		/* top rigth */                            
		if (qnode->x <= lnode->ldx && qnode->y <= lnode->ldy &&
			qnode->x < lnode->rux && qnode->y < lnode->ruy) {
			if (qnode->rtChild == INF) {
				quadTree[*nNodes] = init_node(*nNodes, 0, INF, INF, INF, INF, qnode->n, INF, INF, INF, INF, lnode);
				quadTree[qnode->n].rtChild = *nNodes;	
			} else {
				if (quadTree[qnode->rtChild].location.ldx == 0 && quadTree[qnode->rtChild].location.rux == 0) {	
					add_quad_node(quadTree, &quadTree[qnode->rtChild], lnode, nNodes);
				} else {
					x = (double) (qnode->x + rbx) / 2;
					y = (double) (qnode->y + tby)  / 2;
					quadTree[*nNodes] = init_node(*nNodes, 0, x, y, (x - qnode->x), (y - qnode->y), qnode->n, INF, INF, INF, INF, 0); /* init new part center */
					update_quad_tree(quadTree, &quadTree[*nNodes], &quadTree[quadTree[qnode->n].rtChild], nNodes); /* swap new part center with location */
					*nNodes = *nNodes + 1;
					add_quad_node(quadTree, &quadTree[qnode->rtChild], lnode, nNodes);
				}
			}
		} 
		/* top left */		                            
		if (qnode->x > lnode->ldx && qnode->y <= lnode->ldy &&
			qnode->x >= lnode->rux && qnode->y < lnode->ruy) {			
			if (qnode->ltChild == INF) {
				quadTree[*nNodes] = init_node(*nNodes, 0, INF, INF, INF, INF, qnode->n, INF, INF, INF, INF, lnode);
				quadTree[qnode->n].ltChild = *nNodes;
			} else {
				if (quadTree[qnode->ltChild].location.ldx == 0 && quadTree[qnode->ltChild].location.rux == 0) {
					add_quad_node(quadTree, &quadTree[qnode->ltChild], lnode, nNodes);
				} else {
					x = (double) (qnode->x + lbx) / 2;
					y = (double) (qnode->y + tby)  / 2;
					quadTree[*nNodes] = init_node(*nNodes, 0, x, y, (qnode->x - x), (y - qnode->y), INF, INF, INF, INF, INF, 0); /* init new part center */
					update_quad_tree(quadTree, &quadTree[*nNodes], &quadTree[quadTree[qnode->n].ltChild], nNodes); /* swap new part center with location */
					*nNodes = *nNodes + 1;
					add_quad_node(quadTree, &quadTree[qnode->ltChild], lnode, nNodes);
				}
			}
		} 
		/* bottom right */                           
		if (qnode->x <= lnode->ldx && qnode->y > lnode->ldy &&
			qnode->x < lnode->rux && qnode->y >= lnode->ruy) {
			if (qnode->rbChild == INF) {
				quadTree[*nNodes] = init_node(*nNodes, 0, INF, INF, INF, INF, qnode->n, INF, INF, INF, INF, lnode);
				quadTree[qnode->n].rbChild = *nNodes;
			} else {
				if (quadTree[qnode->rbChild].location.ldx == 0 && quadTree[qnode->rbChild].location.rux == 0) {
					add_quad_node(quadTree, &quadTree[qnode->rbChild], lnode, nNodes);
				} else {                                   
					x = (double) (qnode->x + rbx) / 2;
					y = (double) (qnode->y + bby)  / 2;
					quadTree[*nNodes] = init_node(*nNodes, 0, x, y, (x - qnode->x), (qnode->y - y), qnode->n, INF, INF, INF, INF, 0); /* init new part center */
					update_quad_tree(quadTree, &quadTree[*nNodes], &quadTree[quadTree[qnode->n].rbChild], nNodes); /* swap new part center with location */
					*nNodes = *nNodes + 1;
					add_quad_node(quadTree, &quadTree[qnode->rbChild], lnode, nNodes);
				}
			}
		} 
		/* bottom left */                            
		if (qnode->x > lnode->ldx && qnode->y > lnode->ldy &&
			qnode->x >= lnode->rux && qnode->y >= lnode->ruy) {
			if (qnode->lbChild == INF) {
				quadTree[*nNodes] = init_node(*nNodes, 0, INF, INF, INF, INF, qnode->n, INF, INF, INF, INF, lnode);
				quadTree[qnode->n].lbChild = *nNodes;
			} else {
				if (quadTree[qnode->lbChild].location.ldx == 0 && quadTree[qnode->lbChild].location.rux == 0) {
					add_quad_node(quadTree, &quadTree[qnode->lbChild], lnode, nNodes);
				} else {                                  
					x = (double) (qnode->x + lbx) / 2;
					y = (double) (qnode->y + bby)  / 2;
					quadTree[*nNodes] = init_node(*nNodes, 0, x, y, (qnode->x - x), (qnode->y - y), qnode->n, INF, INF, INF, INF, 0); /* init new part center */
					update_quad_tree(quadTree, &quadTree[*nNodes], &quadTree[quadTree[qnode->n].lbChild], nNodes); /* swap new part center with location */
					*nNodes = *nNodes + 1;
					add_quad_node(quadTree, &quadTree[qnode->lbChild], lnode, nNodes);
				}
			}
		} 	
		
	/* middles */
		struct locationNode divideLocation = *lnode;
		struct locationNode divideLocation2 = *lnode;
		struct locationNode divideLocation3 = *lnode;
		struct locationNode divideLocation4 = *lnode;
		 
		/* right middle */
		if (qnode->x < lnode->ldx && qnode->y > lnode->ldy &&
			qnode->x < lnode->rux && qnode->y < lnode->ruy) {
			divideLocation.ldy = qnode->y;
			add_quad_node(quadTree, &quadTree[qnode->n], &divideLocation, nNodes);
			divideLocation2.ruy = qnode->y;
			*nNodes = *nNodes + 1; 	
			add_quad_node(quadTree, &quadTree[qnode->n], &divideLocation2, nNodes);		
		} 
		/* top middle */
		if (qnode->x > lnode->ldx && qnode->y < lnode->ldy &&
			qnode->x < lnode->rux && qnode->y < lnode->ruy) {
			divideLocation.ldx = qnode->x;
			add_quad_node(quadTree, &quadTree[qnode->n], &divideLocation, nNodes);
			divideLocation2.rux = qnode->x;
			*nNodes = *nNodes + 1; 	
			add_quad_node(quadTree, &quadTree[qnode->n], &divideLocation2, nNodes);		
		}
		/* left middle */
		if (qnode->x > lnode->ldx && qnode->y > lnode->ldy &&
			qnode->x > lnode->rux && qnode->y < lnode->ruy) {
			divideLocation.ldy = qnode->y;
			add_quad_node(quadTree, &quadTree[qnode->n], &divideLocation, nNodes);
			divideLocation2.ruy = qnode->y;
			*nNodes = *nNodes + 1; 	
			add_quad_node(quadTree, &quadTree[qnode->n], &divideLocation2, nNodes);		
		}
		/* bottom middle */
		if (qnode->x > lnode->ldx && qnode->y > lnode->ldy &&
			qnode->x < lnode->rux && qnode->y > lnode->ruy) {
			divideLocation.ldx = qnode->x;
			add_quad_node(quadTree, &quadTree[qnode->n], &divideLocation, nNodes);
			divideLocation2.rux = qnode->x;
			*nNodes = *nNodes + 1; 	
			add_quad_node(quadTree, &quadTree[qnode->n], &divideLocation2, nNodes);		
			
		}
		/* break point in center of location */
		if (qnode->x > lnode->ldx && qnode->y > lnode->ldy &&
			qnode->x < lnode->rux && qnode->y < lnode->ruy) {	
			divideLocation.rux = qnode->x; /* divid left bottom */
			divideLocation.ruy = qnode->y;			
			add_quad_node(quadTree, &quadTree[qnode->n], &divideLocation, nNodes);
			divideLocation2.ldx = qnode->x; /* divid right top */
			divideLocation2.ldy = qnode->y;
			*nNodes = *nNodes + 1; 	
			add_quad_node(quadTree, &quadTree[qnode->n], &divideLocation2, nNodes);				
			divideLocation3.rux = qnode->x; /* divid left top */
			divideLocation3.ldy = qnode->y; 
			*nNodes = *nNodes + 1; 				
			add_quad_node(quadTree, &quadTree[qnode->n], &divideLocation3, nNodes);	
			divideLocation4.ruy = qnode->y; /* divid right bottom */
			divideLocation4.ldx = qnode->x;
			*nNodes = *nNodes + 1;
			add_quad_node(quadTree, &quadTree[qnode->n], &divideLocation4, nNodes);				
		}
	}
}

static void create_tree(struct quadNode *quadTree, struct quadNode *root, struct locationNode *locations, int *locationsNumber, int *nNodes) 
{
	int i, j;
	add_quad_node(quadTree, root, NULL, nNodes);
	*nNodes = *nNodes + 1;
	for (i = 0; i < *locationsNumber; i++) {
		add_quad_node(quadTree, &quadTree[0], &locations[i], nNodes);
		*nNodes = *nNodes + 1;	
	}
}

long cross_product(int ax, int ay, int bx, int by) 
{	
	int cross = (ax * by) - (ay * bx);
	 if (!cross) {	 	
	 	return 0;
	 }
	 return cross > 0 ? 1 : -1;
}

long intersection(long ax, long ay, long bx, long by, long cx, long cy, long dx, long dy, long t1, long t2)
{
	if (cross_product(bx - ax, by - ay, cx - ax, cy - ay) == cross_product(bx - ax, by - ay, dx - ax, dy - ay)) {
		return -1;	
	}
	if ( cross_product(dx - cx, dy - cy, ax - cx, ay - cy) == cross_product(dx - cx, dy - cy, bx - cx, by - cy)) {
		return -1;
	}	
	if ((ax == bx) && (cx != dx)) {
		return t1 + (t2 - t1) * (ax - cx) / (dx - cx);		
	}
	if ((ay == by) && (cy != dy)) {
		return t1 + (t2 - t1) * (ay - cy) / (dy - cy);
	}		
	
	return -1;
}

int find_intersection(long ldx, long ldy, long rux, long ruy, long minT, long maxT, long x1, long y1, long x2, long y2, long t1, long t2, int *enter, int *leave) 
{
	long tmin = -1;
	long t[4];
	int i;
	
	t[0] = intersection(ldx, ldy, rux, ldy, x1, y1, x2, y2, t1, t2);
	t[1] = intersection(rux, ldy, rux, ruy, x1, y1, x2, y2, t1, t2);
	t[2] = intersection(ldx, ruy, rux, ruy, x1, y1, x2, y2, t1, t2);
	t[3] = intersection(ldx, ldy, ldx, ruy, x1, y1, x2, y2, t1, t2);																														                                                                                       
	
	int cnt = 0;
	long temp = 0;
	for (i = 0; i < 4; i++) {
//		printf("t[i]:%d\n", t[i]);
		if (t[i] > 0 && cnt < 1) {
			temp = t[i]; 
			cnt++;
		} else if (t[i] > 0 && cnt == 1) {
			if (temp != t[i]) {
				long diff = abs(temp - t[i]);
				if (diff >= minT && diff <= maxT) {/* path is in location */
					return INF;
				}
				return 0;
			}
		}
	}   
	
	if (temp != 0 && *enter == 1) {
		*leave = 1;
	} else if (temp != 0){
		*enter = 1;
	}              
	                                                            
	return temp;
}

void search_location(struct quadNode *quadTree, int qIndex, struct pathNode pNode_1, struct pathNode pNode_2, long *intersects)
{
	/* in center */
	if (pNode_1.x == quadTree[qIndex].x && pNode_1.y == quadTree[qIndex].y) {
		//what to do what not to do??
	}
	/* right top */
	if (pNode_1.x >= quadTree[qIndex].x && pNode_1.y >= quadTree[qIndex].y && quadTree[qIndex].rtChild != INF) {
		if (quadTree[qIndex].location.ldx == 0 && quadTree[qIndex].location.rux == 0) { /* it's not location */
//			printf("rt:%d\n", quadTree[qIndex].rtChild);
			search_location(quadTree, quadTree[qIndex].rtChild, pNode_1, pNode_2, intersects);
			search_location(quadTree, quadTree[qIndex].rtChild, pNode_2, pNode_1, intersects);
		} 
	} 
	/* left top */
	if (pNode_1.x <= quadTree[qIndex].x && pNode_1.y >= quadTree[qIndex].y && quadTree[qIndex].ltChild != INF) {
		if (quadTree[qIndex].location.ldx == 0 && quadTree[qIndex].location.rux == 0) { /* it's not location */
//			printf("lt:%d\n", quadTree[qIndex].ltChild);
			search_location(quadTree, quadTree[qIndex].ltChild, pNode_1, pNode_2, intersects);
			search_location(quadTree, quadTree[qIndex].ltChild, pNode_2, pNode_1, intersects);				
		} 
	}
	/* right bottom */
	if (pNode_1.x >= quadTree[qIndex].x && pNode_1.y <= quadTree[qIndex].y && quadTree[qIndex].rbChild != INF) {
		if (quadTree[qIndex].location.ldx == 0 && quadTree[qIndex].location.rux == 0) { /* it's not location */
//			printf("rb:%d\n", quadTree[qIndex].rbChild);
			search_location(quadTree, quadTree[qIndex].rbChild, pNode_1, pNode_2, intersects);
			search_location(quadTree, quadTree[qIndex].rbChild, pNode_2, pNode_1, intersects);
		} 
	}
	/* left bottom */
	if (pNode_1.x <= quadTree[qIndex].x && pNode_1.y <= quadTree[qIndex].y && quadTree[qIndex].lbChild != INF) {
		if (quadTree[qIndex].location.ldx == 0 && quadTree[qIndex].location.rux == 0) { /* it's not location */
//			printf("lb:%d\n", quadTree[qIndex].lbChild);
			search_location(quadTree, quadTree[qIndex].lbChild, pNode_1, pNode_2, intersects);
			search_location(quadTree, quadTree[qIndex].lbChild, pNode_2, pNode_1, intersects);					
		} 
	}
	
	/* it is location */
	if (quadTree[qIndex].location.ldx == 0 && quadTree[qIndex].location.rux == 0) {
		printf("");
	} else {
		intersects[quadTree[qIndex].location.id] = 1; 	
	}
}

static double min_max(double *val1, double *val2, int op) 
{
   if (op == 0) { /* return greater */
   	    if (*val1 > *val2) {
   	    	return *val1;
		}
		return *val2;
   } else { /* return lower */
   		if (*val2 > *val1) {
   			return *val1;
		}
		return *val2;
   }
}

void display(struct quadNode *quadTree, int nNodes) 
{
	int i;
	
	printf("nodes: %d\n", nNodes);
	for (i = 0; i < nNodes; i++) {
		printf("\nn:%d -> x: %f, y:%f, diffX:%f, diffY:%f, parent:%d, rtChild:%d, rbChild:%d, ltChild:%d, lbChild:%d\n", 
			quadTree[i].n, quadTree[i].x, quadTree[i].y, quadTree[i].diffX, quadTree[i].diffY, quadTree[i].parent, quadTree[i].rtChild, quadTree[i].rbChild, quadTree[i].ltChild, quadTree[i].lbChild);
		if (&quadTree[i].location != NULL) {
			printf("location-> ldx:%lf, ldy:%lf, rux:%lf, ruy:%lf\n", quadTree[i].location.ldx, quadTree[i].location.ldy, quadTree[i].location.rux, quadTree[i].location.ruy);
		}
	}
}

struct quadNode read_locations(FILE *rf, struct locationNode *locations) 
{
	struct quadNode root;
	double minX = 100000, minY = 100000, maxX = -10000, maxY = -100000;
	char line[1024], cpy[16];
	int i = 0;
	
	while (fgets(line, sizeof(line), rf)) {
		int charCounter = 0;
		int cpyCharCounter = 0;
		
		locations[i].id = i;
		memset(&cpy[0], 0, sizeof(cpy));

		while (line[charCounter] != ' ') {
			cpy[cpyCharCounter] = line[charCounter];
			charCounter++;
			cpyCharCounter++;
		}
		locations[i].ldx = atol(cpy);
		minX = min_max(&minX, &locations[i].ldx, 1);
		memset(&cpy[0], 0, sizeof(cpy));
		charCounter++;
		cpyCharCounter = 0;
		
		while (line[charCounter] != ' ') {
			cpy[cpyCharCounter] = line[charCounter];
			charCounter++;
			cpyCharCounter++;
		}
		locations[i].ldy = atol(cpy);
		minY = min_max(&minY, &locations[i].ldy, 1);
		memset(&cpy[0], 0, sizeof(cpy));
		charCounter++;
		cpyCharCounter = 0;
		
		while (line[charCounter] != ' ') {
			cpy[cpyCharCounter] = line[charCounter];
			charCounter++;
			cpyCharCounter++;
		}
		locations[i].rux = atol(cpy);
		maxX = min_max(&maxX, &locations[i].rux, 0);
		memset(&cpy[0], 0, sizeof(cpy));
		charCounter++;
		cpyCharCounter = 0;
		
		while (line[charCounter] != ' ') {
			cpy[cpyCharCounter] = line[charCounter];
			charCounter++;
			cpyCharCounter++;
		}
		locations[i].ruy = atol(cpy);
		maxY = min_max(&maxY, &locations[i].ruy, 0);
		memset(&cpy[0], 0, sizeof(cpy));
		charCounter++;
		cpyCharCounter = 0;
		
		while (line[charCounter] != ' ') {
			cpy[cpyCharCounter] = line[charCounter];
			charCounter++;
			cpyCharCounter++;
		}
		locations[i].minT = atol(cpy);
		memset(&cpy[0], 0, sizeof(cpy));
		charCounter++;
		cpyCharCounter = 0;
		
		while (line[charCounter] != ' ') {
			cpy[cpyCharCounter] = line[charCounter];
			charCounter++;
			cpyCharCounter++;
		}
		locations[i].maxT = atol(cpy);
		memset(&cpy[0], 0, sizeof(cpy));
		charCounter++;
		cpyCharCounter = 0;
		
		i++;		
	}                     
	
	double x = (double) (maxX + minX) / 2;
	double y = (double) (maxY + minY)  / 2;
	root = init_node(0, 1, x, y, (x - (double) minX), (y - (double) minY), INF, INF, INF, INF, INF, 0);	
	
	return root;
}

static void read_path(FILE *rf, struct pathNode *path, int pathNodes)
{
	int i = 0;
	char line[1024], cpy[16];
	for (i = 0; i < pathNodes; i++) {
		fgets(line, sizeof(line), rf);
		int charCounter = 0;
		int cpyCharCounter = 0;
		while (line[charCounter] != ' ') {
			cpy[cpyCharCounter] = line[charCounter];
			charCounter++;
			cpyCharCounter++;
		}
		path[i].t = atol(cpy);
		memset(&cpy[0], 0, sizeof(cpy));
		charCounter++;
		cpyCharCounter = 0;
		
		while (line[charCounter] != ' ') {
			cpy[cpyCharCounter] = line[charCounter];
			charCounter++;
			cpyCharCounter++;
		}
		path[i].x = atol(cpy);
		memset(&cpy[0], 0, sizeof(cpy));
		charCounter++;
		cpyCharCounter = 0;
		
		while (line[charCounter] != ' ') {
			cpy[cpyCharCounter] = line[charCounter];
			charCounter++;
			cpyCharCounter++;
		}
		path[i].y = atol(cpy);
		memset(&cpy[0], 0, sizeof(cpy));
//		printf("%ld, %ld, %ld\n", path[i].t, path[i].x, path[i].y);	
	}
}	

static long timestamp(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main () 
{
	int locationsNumber, pathsNumber, i, j;
	int nNodes = 0;
	struct locationNode *locations;
	struct paths *npaths;
	struct quadNode root;
	struct quadNode *quadTree;
	FILE *locationsFile, *pathFile, *writeFile;
	char line[1024];
	long ts0, ts1, ts2, ts3, ts4;
	struct timeval stop, start;
		
	writeFile = fopen("ans.txt", "w");	
	locationsFile = fopen("locations.txt", "r");
	pathFile = fopen("paths.txt", "r");
			
	if (!locationsFile) {
		printf("no file");
	} 
	if (!pathFile) {
		printf("no p file");	
	}	
	
	ts0 = timestamp(); /* time of reading locations and paths */
	
	fgets(line, sizeof(line), locationsFile);
	locationsNumber = atoi(line);
	locations = malloc(locationsNumber * sizeof(locations[0]));
	root = read_locations(locationsFile, locations);		
	
	fgets(line, sizeof(line), pathFile);
	pathsNumber = atoi(line);
	npaths = malloc(pathsNumber * sizeof(npaths[0]));
	for (i = 0; i < pathsNumber; i++) {
		fgets(line, sizeof(line), pathFile);
		npaths[i].n = atoi(line); 
		npaths[i].pathNodes = malloc(npaths[i].n * sizeof(npaths[i].pathNodes[0]));
		read_path(pathFile, npaths[i].pathNodes, npaths[i].n);	
	}
	
	/* create quad tree */
	ts1 = timestamp(); /* time of creating tree */
	gettimeofday(&start, NULL);
	quadTree = malloc(locationsNumber * 10 * sizeof(quadTree[0]));
	create_tree(quadTree, &root, locations, &locationsNumber, &nNodes);
//	display(quadTree, nNodes); /* display tress's nodes */
	gettimeofday(&stop, NULL);
		
	int topBoundry = root.y + root.diffY;
	int bottomBoundry = root.y - root.diffY;
	int rightBoundry = root.x + root.diffX;
	int leftBoundry = root.x - root.diffX;		
	int *visits = malloc(locationsNumber * sizeof(int));
	long *intersects = malloc(locationsNumber * sizeof(intersects[0]));
	for (i = 0; i < locationsNumber; i++) {
		visits[i] = 0;
	}
	
	/* answer questions */
	ts2 = timestamp(); /* time of answergin the questions */
	int n = 0;
	while (n < pathsNumber) {
		for (i = 0; i < locationsNumber; i++) {
			intersects[i] = 0;
		}

		for (i = 0; i < npaths[n].n - 1; i++) {
			if (npaths[n].pathNodes[i].x >= leftBoundry && npaths[n].pathNodes[i].x <= rightBoundry || npaths[n].pathNodes[i + 1].x >= leftBoundry && npaths[n].pathNodes[i + 1].x <= rightBoundry) {
				if (npaths[n].pathNodes[i].y >= bottomBoundry && npaths[n].pathNodes[i].y <= topBoundry || npaths[n].pathNodes[i + 1].y >= bottomBoundry && npaths[n].pathNodes[i + 1].y <= topBoundry) {
					search_location(quadTree, 0, npaths[n].pathNodes[i], npaths[n].pathNodes[i + 1], intersects);
				}
			}
		}
					
		int enter = 0;
		int leave = 0;	
		int temp = 0;
		for (i = 0; i < npaths[n].n - 1; i++) {
			for (j = 0; j < locationsNumber; j++) {
				if (intersects[j] != 0) {
					long t = find_intersection(locations[j].ldx, locations[j].ldy, locations[j].rux, locations[j].ruy, locations[j].minT, locations[j].maxT,
										npaths[n].pathNodes[i].x, npaths[n].pathNodes[i].y, npaths[n].pathNodes[i + 1].x, npaths[n].pathNodes[i + 1].y, npaths[n].pathNodes[i].t, npaths[n].pathNodes[i + 1].t, &enter, &leave);
					t = abs(t);
					if (t == INF) {
						enter = 0;
						leave = 0;
						visits[j]++;		
					} else if (t > 0 && enter == 1 && leave == 0) {
						temp = t;
					} else if (t > 0 && enter == 1 && leave == 1) {
						long diff = abs(temp - t);
						if (diff >= locations[j].minT && diff <= locations[j].maxT) {	
							visits[j]++;
						}
						enter = 0;
						leave = 0;
					}
				}	
			}
		}
		n++;
	}
		
	for (i = 0; i < locationsNumber; i++) {
		fprintf(writeFile, "%d\n", visits[i]);
	}
	
	ts3 = timestamp(); /* finsih time */
	
	printf("\n\n%ld, %ld, %ld", ts1 - ts0, (stop.tv_sec - start.tv_sec) * 1000  + stop.tv_usec - start.tv_usec, ts3 - ts2);
		
	free(npaths);
	free(locations);
	free(quadTree);
}
