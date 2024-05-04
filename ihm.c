#include "ihm.h"
#include<graph.h>
static couleur white,pluie;
void startIHM(int w,int h)
{
	InitialiserGraphique();
	CreerFenetre((Maxx()-w)/2,(Maxy()-h)/2,w,h);
	white = CouleurParNom("white");
	pluie = CouleurParComposante(16,52,166); // g changé j'étais perturbée qu la pluie était noire
}
void pointIHM(int x,int y,colorIHM c)
{
	couleur cc = (c == WHITE)?white:pluie;
	ChoisirCouleurDessin(cc);
	DessinerPixel(x,y);
}
void lineIHM(int x,int y,int xx,int yy,colorIHM c)
{
	couleur cc = (c == WHITE)?white:pluie;
	ChoisirCouleurDessin(cc);
	DessinerSegment(x,y,xx,yy);
}
void endIHM()
{
	FermerGraphique();
}
unsigned long microsecondesIHM()
{
	return Microsecondes();
}