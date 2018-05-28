#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>

#define WORD_LEN 8
#define WORD_COUNT 1000
int words[WORD_COUNT][WORD_LEN];
#define MEMBER_COUNT 30
#define SURVIVE_COUNT 8
#define MUTATE_COUNT 5
#define ITER_COUNT 1000

int fitness(int* member) {
  int fitness = 0;
  for(int wordNum = 0; wordNum < WORD_COUNT; wordNum++) {
    int *word = words[wordNum];
    for(int firstChar = 0; firstChar < WORD_LEN; firstChar++) {
      fitness += 26 - abs(word[firstChar] - member[firstChar]); //26 - distance between chars
      if(word[firstChar] == member[firstChar]) { //if chars are same
        fitness += 10;
        if(firstChar < WORD_LEN - 1) { //if there is another char after this one
          fitness += 2 * (26 - abs(word[firstChar + 1] - member[firstChar + 1])); //2 * (26 - distance between chars)
          if(word[firstChar + 1] == member[firstChar + 1]) { //if the second chars are the same
            fitness += 40;
            if(firstChar < WORD_LEN - 2) { //if there is a third char
              fitness += 4 * (26 - abs(word[firstChar + 2] - member[firstChar + 2])); //4 * (26 - distance between chars)
              if(word[firstChar + 2] == member[firstChar + 2]) { //if third chars are the same
                fitness += 160;
              }
            }
          }
        }
      }
    }
  }
  return fitness;
}
void genMember(int* dest) {
  for(int i = 0; i < WORD_LEN; i++) {
    dest[i] = rand() % 26;
  }
}

int membersBase[MEMBER_COUNT][WORD_LEN];
int fitnessBase[MEMBER_COUNT];
#ifdef DEBUG
int fitnessSorted[SURVIVE_COUNT];
#endif
int membersBest[SURVIVE_COUNT][WORD_LEN];
//https://stackoverflow.com/a/10584944
int compare(const void *a, const void *b) {
  double diff = fitnessBase[*(int*)a] - fitnessBase[*(int*)b];
  return  (0 < diff) - (diff < 0);
}

int sortByFitness() {
  int perm[MEMBER_COUNT];
  int i;
  for(i = 0; i < MEMBER_COUNT; i++) {
    perm[i] = i;
  }
  qsort(perm, MEMBER_COUNT, sizeof(int), compare);
  for (i = 0; i < SURVIVE_COUNT; i++) {
    memcpy(membersBest[i], membersBase[perm[MEMBER_COUNT - i - 1]], sizeof(int) * WORD_LEN);
#ifdef DEBUG
    fitnessSorted[i] = fitnessBase[perm[MEMBER_COUNT - i - 1]];
#endif
  }
}

int main() {
  srand(time(NULL));
  
  FILE *fp;
  char command[100];
  sprintf(command, "/bin/bash words.sh %d", WORD_LEN);
  fp = popen(command, "r");
  int i = 0;
  memset(words, 0, sizeof(words));
  char buf[WORD_LEN + 1];
  while(fgets(buf, WORD_LEN + 1, fp) != NULL && i < WORD_COUNT) {
    for(int n = 0; n < WORD_LEN; n++) {
      words[i][n] = buf[n] - 'a';
    }
    i++;
    fgets(buf, 2, fp);
  }
  pclose(fp);
  
  //Initial population (size MEMBER_COUNT)
  for(int i = 0; i < MEMBER_COUNT; i++) {
    genMember(membersBase[i]);
  }
  
  for(int iter = 0; iter < ITER_COUNT; iter++) {
    //Calculate fitness
    for(int i = 0; i < MEMBER_COUNT; i++) {
      fitnessBase[i] = fitness(membersBase[i]);
    }
    
    //Select top SURVIVE_COUNT
    sortByFitness();
    
#ifdef DEBUG
    //Print to stdout
    if(iter < 999) {
      //for(int i = 0; i < SURVIVE_COUNT; i++) {
        int i = 0;
        char buf[WORD_LEN + 1];
        for(int c = 0; c < WORD_LEN; c++) {
          buf[c] = membersBest[i][c] + 'a';
        }
        buf[WORD_LEN] = 0; //null terminate the string
        printf("%d %s %d\n", iter, buf, fitnessSorted[i]);
      //}
      //return 0;
    }
#endif
    
    //Random interbreeding to create MEMBER_COUNT new members
    for(int i = 0; i < MEMBER_COUNT; i++) {
      int *memberA = membersBest[rand() % SURVIVE_COUNT];
      int *memberB = membersBest[rand() % SURVIVE_COUNT];
      int memberNew[WORD_LEN];
      for(int n = 0; n < WORD_LEN; n++) {
        memberNew[n] = (int)((memberA[n] + memberB[n]) / 2);
      }
      memcpy(membersBase[i], memberNew, sizeof(int) * WORD_LEN);
    }
    
    //Mutation
    for(int i = 0; i < MUTATE_COUNT; i++) {
      int *member = membersBase[rand() % MEMBER_COUNT];
      int letterNum = rand() % WORD_LEN;
      int newLetter = member[letterNum] + ((rand() % 11) - 5); //Add random number in range (-5, 5)
      //Constrain to range (0, 25)
      newLetter = newLetter < 0 ? newLetter + 26 : newLetter;
      newLetter = newLetter > 25 ? newLetter - 26 : newLetter;
      member[letterNum] = newLetter;
    }
  }
  
  //Print to stdout
  //char buf[WORD_LEN + 1];
  for(int c = 0; c < WORD_LEN; c++) {
    buf[c] = membersBase[0][c] + 'a';
  }
  buf[WORD_LEN] = 0; //null terminate the string
  printf("%s\n", buf);
  
  return 0;
}
