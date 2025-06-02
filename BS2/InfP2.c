#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>

// Min und Max Collatz
int minCollatz = 1;
int maxCollatz = 100000000;

// Zeit der Sequentiellen und der Parallelen Berechnung
float Seq;
float Par;
// Structs
typedef struct StaEndBer{
  int Start;
  int End;
}StaEndBer;

typedef struct staWertlaenFol{
  int staWert;
  int laenFol;
}staWertlaenFol;

staWertlaenFol Laengste;

void seqCollatz(){
  printf("Sequenzielle Berechnung: \n");
  // Zeitberechnung
  int tBegin;
  int tBeginNano;
  int tEnd;
  int tEndNano;
  struct timespec ts; 
  clock_gettime(CLOCK_REALTIME, &ts);
  tBegin = ts.tv_sec;
  tBeginNano = ts.tv_nsec;
  // Begin Collatz 1-100.000.000
  int cnt;
  for(int i = minCollatz; i< maxCollatz; i++) {
    int x = i;
    cnt = 0;
    while(x > 1){
      if(x % 2 == 0){
        x = x / 2;
      }else{
        x =(3 * x)+ 1;
      }
      cnt++;
    }
  }
  Laengste.staWert = 1;
  Laengste.laenFol = cnt;
  
  clock_gettime(CLOCK_REALTIME, &ts);
  tEnd = ts.tv_sec;
  tEndNano = ts.tv_nsec;

  Seq = (tEnd - tBegin)+((tEndNano - tBeginNano)/1e9);
  printf("nach %f Sekunden abgeschlossen\n\n", Seq);
}

// RechenFunktion der Threads
void *SpeedUp(void *args){
  StaEndBer * CurrRange = (StaEndBer *) args;
  int cnt = 0;
  for(int z = CurrRange->Start; z <= CurrRange->End; z++){
    cnt = 0; 
    int x = z;
    while( x > 1){
      if( x % 2 == 0){
        x = x / 2;
      }else{
        x = 3 * x + 1;
      }
      cnt++;
    }
    if(cnt > Laengste.laenFol){
      Laengste.laenFol = cnt;
      Laengste.staWert = CurrRange->Start;
    }
  }
}
// Parallele Berechnung
void parCollatz(int AnzThreads){
  printf("parallele Berechnung mit %i Threads. \n", AnzThreads);
  int tBegin;
  int tBeginNano;
  int tEnd;
  int tEndNano;

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  tBegin = ts.tv_sec;
  tBeginNano = ts.tv_nsec;
  
  //Länge jeder zu Übergebenden Collatzberechunung berechnen
  int laenge = (maxCollatz - minCollatz)/AnzThreads;
  StaEndBer StaEnd[AnzThreads];
  for(int i = 0; i < AnzThreads; i++){
    if( i == AnzThreads - 1){
      StaEnd[i].Start = minCollatz + laenge*i;
      StaEnd[i].End = maxCollatz;
    }else{
      StaEnd[i].Start = minCollatz + laenge*i;
      StaEnd[i].End = maxCollatz/AnzThreads + laenge*i;
    }
  }
  pthread_t threads[AnzThreads];
  uintptr_t threadRetParam = -1;

  struct timespec tsThread[AnzThreads];
  int tBeginThread[AnzThreads];
  int tBeginThreadNano[AnzThreads];
  for(int i = 0; i < AnzThreads; i++){
    clock_gettime(CLOCK_REALTIME, &tsThread[i]);
    pthread_create(&threads[i], NULL, &SpeedUp, &StaEnd[i]);
    tBeginThread[i] = tsThread[i].tv_sec;
    tBeginThreadNano[i] = tsThread[i].tv_nsec;
    printf("Thread %d created\n", i);
  }
  int tEndThread[AnzThreads];
  int tEndThreadNano[AnzThreads];
  for(int i = 0; i < AnzThreads; i++){
    pthread_join(threads[i], (void *)(&threadRetParam));
    clock_gettime(CLOCK_REALTIME, &tsThread[i]);
    tEndThread[i]=tsThread[i].tv_sec;
    tEndThreadNano[i]=tsThread[i].tv_nsec;
    printf("Thread %d finished after running for %f Seconds, calculating Collatz sequenz: %d - %d \n", i, tEndThread[i]-tBeginThread[i] + ((tEndThreadNano[i] - tBeginThreadNano[i])/1e9), StaEnd[i].Start, StaEnd[i].End); 
  }
  clock_gettime(CLOCK_REALTIME, &ts);
  tEnd = ts.tv_sec;
  tEndNano = ts.tv_nsec;
  Par = tEnd-tBegin + (tEndNano - tBeginNano)/1e9;
  printf("Total Runtime of %f Seconds for %d Threads with %f times the Speed of sequential\n\n", Par, AnzThreads, Seq/Par); 
}

int main(){
  seqCollatz();
  for(int a = 2; a < 17; a++){ 
    parCollatz(a);
  }
  return 0;
}