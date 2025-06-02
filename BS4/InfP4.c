#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>
#include <stdatomic.h>
#include <semaphore.h>


// Min und Max Collatz
int minCollatz = 1;
uint64_t maxCollatz = 100000000;

// aktuell zu Berechnende Zahl
atomic_int akt = 1;
uint64_t Sum = 0;

sem_t sem;

pthread_mutex_t lm = PTHREAD_MUTEX_INITIALIZER; // lm = Lock Mutex

// Zeit der Sequentiellen und der Parallelen Berechnung
float Seq;
float Par;
// Structs
/*typedef struct StaEndBer{
  int Start;
  int End;
}StaEndBer;*/
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
  uint32_t cnt;
  for(int i = minCollatz; i< maxCollatz; i++) {
    uint64_t x = i;
    cnt = 0;
    while(x > 1){
      if(x % 2 == 0){
        x = x / 2;
      }else{
        x =(3 * x)+ 1;
      }
      cnt++;
    }
    Sum += cnt;
  }
  Laengste.staWert = 1;
  Laengste.laenFol = cnt;
  
  clock_gettime(CLOCK_REALTIME, &ts);
  tEnd = ts.tv_sec;
  tEndNano = ts.tv_nsec;

  Seq = (tEnd - tBegin)+((tEndNano - tBeginNano)/1e9);
  printf("nach %f Sekunden abgeschlossen\n\n", Seq);
  printf("Collatzsumme: %li \n", Sum);
  printf("Kontrolsumme: 17923493583\n");
}

// RechenFunktion der Threads
void *SpeedUp(void *args){
  char control = *(char *) args; 
  uint64_t x;
  while(1){
    if(akt >= maxCollatz){break;}
    if(control == 'a'){        
        x = atomic_fetch_add( &akt, 1 ); 
    }else if(control == 's'){
        sem_wait(&sem);
        x = akt;
        akt++;
        sem_post(&sem);    
    }else if(control == 'm'){
        pthread_mutex_lock(&lm);
        x = akt;
        akt++;
        pthread_mutex_unlock(&lm);
    }else{
        printf("Kein Collatz-Rechenverfahren gefunden \n");
        return 0;
    }
    uint32_t cnt = 0;
    //printf("aktuell zu berechnen: %i\n", x);
    while( x > 1){
      if( x % 2 == 0){
        x = x / 2;
      }else{
        x = 3 * x + 1;
      }
      cnt++;
    }
    atomic_fetch_add(&Sum, cnt);
    //printf("%lli (+ %i)\n", Sum, cnt);
  }
  return NULL;
}   

// Parallele Berechnung
void parCollatz(int AnzThreads, char LV){
  printf("parallele Berechnung mit %i Threads. \n", AnzThreads);
  int tBegin;
  int tBeginNano;
  int tEnd;
  int tEndNano;

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  tBegin = ts.tv_sec;
  tBeginNano = ts.tv_nsec;
  // Angeben, ob Mutex, Atomar oder Semaphore
  if(LV == 's'){
    printf("Berechnung mit Semaphoren\n");
    sem_init(&sem, 0, 1);
  }else if(LV == 'a'){
    printf("Berechnung mit Atomics\n");
  }else if (LV == 'm'){
    printf("Berechnung mit Mutex\n");
  }

  pthread_t threads[AnzThreads];
  uintptr_t threadRetParam = -1;

  struct timespec tsThread[AnzThreads];
  int tBeginThread[AnzThreads];
  int tBeginThreadNano[AnzThreads];
  for(int i = 0; i < AnzThreads; i++){
    clock_gettime(CLOCK_REALTIME, &tsThread[i]);
    pthread_create(&threads[i], NULL, &SpeedUp, &LV);
    tBeginThread[i] = tsThread[i].tv_sec;
    tBeginThreadNano[i] = tsThread[i].tv_nsec;
    printf("Thread %d created\n", i+1);
  }
  int tEndThread[AnzThreads];
  int tEndThreadNano[AnzThreads];
  for(int i = 0; i < AnzThreads; i++){
    pthread_join(threads[i], (void **)(&threadRetParam));
    clock_gettime(CLOCK_REALTIME, &tsThread[i]);
    tEndThread[i]=tsThread[i].tv_sec;
    tEndThreadNano[i]=tsThread[i].tv_nsec;
    printf("Thread %d finished ran for %f Seconds \n", i+1, tEndThread[i]-tBeginThread[i] + ((tEndThreadNano[i] - tBeginThreadNano[i])/1e9)); 
  }
  if(LV == 's'){
    sem_destroy(&sem);
  }
  clock_gettime(CLOCK_REALTIME, &ts);
  tEnd = ts.tv_sec;
  tEndNano = ts.tv_nsec;
  Par = tEnd-tBegin + (tEndNano - tBeginNano)/1e9;
  printf("Total Runtime of %f Seconds for %d Threads\n\n", Par, AnzThreads);
  printf("Collatzsumme: %li \n", Sum);
  printf("Kontrolsumme: 17923493583\n");
}

int main(){
  char c;
  int a;
  printf("Semaphore(s), Atomic(a) oder Mutex(m)?\n" );
  scanf(" %c", &c);
  //seqCollatz();
  printf("Wieviele Threads sollen verwendet werden? (1-16)\n");
  scanf(" %i", &a);
  parCollatz(a, c);
  
  return 0;
}