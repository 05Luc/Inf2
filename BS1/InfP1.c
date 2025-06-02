#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>

#define MAX_LEN 100

typedef struct Name{
  char Name[MAX_LEN];
  uintptr_t NameLen;
} Name;

/*  Thread-Funktion */
void *SetNameLen(void *args){
  /* Die Übergabe wird zurück auf einen int Pointer gecastet */
  Name *inName = (Name *) args;
  inName->NameLen = strlen(inName->Name);
  return 0;
}

int main(){
  /* Lege ein Thread-Handle, einen Übergabe- und einen Rückgabeparameter an */
  pthread_t thread;
  uintptr_t threadRetParam = -1;

  
  Name MyName;

  strncpy(MyName.Name, "Lucas", MAX_LEN);

  /* Starte einen Thread mit der auszuführenden Funktion example_fct.
   * Zudem wird ein Paramter übergeben.
   * Konfigurations-Parameter werden nicht gesetzt, daher NULL. */
  pthread_create(&thread, NULL, &SetNameLen, &MyName);

  /* Warte auf Beendigung des Threads */
  pthread_join(thread, (void **)(&threadRetParam));

  /* Inhalt des Rückgabeparameters ausgeben */
  printf("Anzahl Zeichen in Name (Name=%s): %i\n",MyName.Name, MyName.NameLen);
  printf("%i\n", threadRetParam);
  return 0;
}
