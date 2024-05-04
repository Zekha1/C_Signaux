#include "ihm.h"
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>
#include<signal.h>
#include<assert.h>
#define XRANG 800
#define YRANG 400
#define CYCLE 50L  // Periode
#define VR 2      // Mise à jour essuie glace rapide VR*CYCLE
#define VL 4     // Mise à jour essuie glace lente  VL*CYCLE
#define NPLUIE  9 // controle la pluie 0 faible -> 10 fort
typedef enum{
	ARRET,
	RAPIDE,
	LENT
}  MODE;
MODE Mode = ARRET;
int imp = 0,MARCHE=1,sens=1;
double 	inc = -0.001;
void handsig(int sig){
	switch(sig){
		case SIGTERM : 
			MARCHE = 0;
			break;
		case SIGUSR1 : 
			imp = 1;
			break;
		case SIGHUP : // lent
			Mode = LENT;
			break;
		case SIGPIPE : // rapide
			Mode = RAPIDE;
			break;
		case SIGUSR2 : 
			if (Mode == ARRET) Mode = LENT;
			else
				Mode = ARRET;
			break;
			// ARRET
	}
}
void pluie(int w,int h)
{
	int i;
	for (i = 0; i < rand() % 10; i++)
	{
		/* point noir */
		pointIHM(rand() % w, rand() % h, BLACK);
	}
	for (i = 0; i < rand() % 100; i++)
	{
		/* point blanc */
		pointIHM(rand() % w, rand() % h, WHITE);
	}
}
void essuieglace(){
	static int X1,X2,longueurSegment = 300,x,y,memx=-1,memy=-1;
	static double angle=-0.01;
	X1 = (longueurSegment / 2) + 10;
	X2 = (X1 + longueurSegment + (longueurSegment / 10));
	if (angle >= -0.01 && Mode == ARRET && imp == 0) return;
	//for (angle = -0.01; angle < 0.0; angle += inc)
	/* Passage coordonnees polaires --> cartesiennes */
	x = (short) (longueurSegment * cos(angle));
	y = (short) (longueurSegment * sin(angle));
	lineIHM(X1, YRANG, (X1 + x), (YRANG + y), BLACK);
	lineIHM(X2, YRANG, (X2 + x), (YRANG + y), BLACK);
	/* Effacement de la ligne precedante */
	if (x != memx || y != memy){
		lineIHM(X1, YRANG, (X1 + memx), (YRANG + memy), WHITE);
		lineIHM(X2, YRANG, (X2 + memx), (YRANG + memy), WHITE);
	}
	/* Memorisation des positions pour l'effacement */
	memx = x;
	memy = y;
	/* Deplacement maximum du segment */
	if (angle < -(2 * (M_PI / 3))|| angle > -0.01){
		sens = -sens;
	}
	if (angle > -0.01 && imp) imp = 0;
	angle+=sens*inc;
}
void mapsignal(){
	struct sigaction sa = {0};
	sa.sa_flags = 0;
	sa.sa_handler = handsig ;
	sigset_t sigB;
	sigemptyset(&sigB);
	sigaddset(&sigB,SIGUSR1);
	sigaddset(&sigB,SIGUSR2);
	sigaddset(&sigB,SIGHUP);
	sigaddset(&sigB,SIGPIPE);
	sigaddset(&sigB,SIGTERM);
	sa.sa_mask = sigB;
	assert( sigaction(SIGUSR1,&sa,NULL) == 0 );
	assert( sigaction(SIGHUP,&sa,NULL)  == 0 );
	assert( sigaction(SIGPIPE,&sa,NULL) == 0 );
	assert( sigaction(SIGUSR2,&sa,NULL) == 0 );
	assert( sigaction(SIGTERM,&sa,NULL) == 0 );
	sigemptyset(&sigB);
	sigaddset(&sigB,SIGINT);
	sigaddset(&sigB,SIGTSTP);
	sigaddset(&sigB,SIGQUIT);
	assert(sigprocmask(SIG_BLOCK,&sigB,NULL) == 0);
}
int main(int argc, char *argv[])
{
	unsigned long suivant;
	int topEss, topEssMAJ=VR; // topEss == nbre de cycle restant avant la MAJ, topEssMAJ == periode de MAJ
	printf("PID = %d\n",getpid());
	mapsignal();
	startIHM(XRANG,YRANG);
	topEss = VL;
	suivant = microsecondesIHM() + CYCLE;
	while(MARCHE){
		if (microsecondesIHM() > suivant){
			topEss-- ;
			if (topEss == 0) {
				essuieglace();
				topEss = topEssMAJ;
			}
			if (rand() % 10 <=NPLUIE){
				pluie(XRANG,YRANG);
			}
			suivant= microsecondesIHM()+CYCLE;
		}
		switch (Mode){
			case RAPIDE:
				topEssMAJ=VR;
				break;
			case LENT:
				topEssMAJ=VL;
				break;
			case ARRET:
				topEssMAJ=VL;
				break;
		}
	}
	endIHM();	
	return 0;
}