#ifndef _IHM_H
#define _IHM_H
typedef enum { WHITE,BLACK  } colorIHM ;
void startIHM(int w,int h);
void endIHM();
void pointIHM(int x,int y,colorIHM c);
void lineIHM(int x,int y,int xx,int yy,colorIHM c);
unsigned long microsecondesIHM(); 
#endif