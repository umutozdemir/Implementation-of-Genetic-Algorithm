#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Gene {
    struct Gene *nextGene ;
    int data ;
};

struct Chromosome {
    struct Chromosome *nextChromosome ;
    struct Gene *headGene  ;
    int fitness ;
    double rank ;
};

struct Chromosome *headChromosome ;
int bestFitness ;
int worstFitness ;
struct Chromosome *bestChromosome ;

struct Gene* createGene(int data,struct Gene *head) {

    struct Gene *temp1 = (struct Gene*) malloc(sizeof(struct Gene)) ;
    temp1 -> data = data ;
    temp1 -> nextGene = NULL ;
    if(head == NULL) {
        head = temp1 ;
        return  head ;
    }
    struct Gene *temp2 = head  ;
    while(temp2->nextGene != NULL) {
        temp2 = temp2 -> nextGene ;
    }
    temp2 ->nextGene = temp1 ;
    return head ;
}

void createChromosome(char *line) {

    struct Chromosome *temp1 = (struct Chromosome*)malloc(sizeof(struct Chromosome)) ;
    temp1 ->headGene = NULL ;
    temp1 ->nextChromosome = NULL ;
    temp1 -> fitness = 0 ;
    if(headChromosome == NULL) {
        headChromosome = temp1 ;
    }
    else {
        struct Chromosome *temp2 = headChromosome ;
        while(temp2->nextChromosome != NULL) {
            temp2 = temp2 -> nextChromosome ;
        }
        temp2 -> nextChromosome = temp1 ;
    }
    char *token ;
    token = strtok(line,":") ;
    while(token != NULL) {
        int genData = atoi(token) ;
        temp1->headGene = createGene(genData,temp1->headGene) ;
        token = strtok(NULL,":\n\r") ;
    }
}

int myPow(int x,int y) {
    int result = 1 ;
    if(y==0) {
        return 1;
    }
    int j ;
    for(j=0;j<y;j++) {
        result = result*x ;
    }
    return result ;
}

void fitnessCalculation(int prob_size) {
    struct Chromosome *temp = headChromosome;
    int i ;
    while(temp != NULL) {
        temp -> fitness = 0 ;
        struct Gene *tempGene = temp -> headGene ;
        i = prob_size-1 ;
        while(tempGene != NULL) {
            temp -> fitness += (tempGene->data) * (myPow(2,i)) ;
            i-- ;
            tempGene = tempGene ->nextGene ;
        }
        temp = temp ->nextChromosome;
    }
}

void findBestWorstFitness() {
    struct Chromosome *temp = headChromosome ;
    while(temp != NULL) {
        if(temp->fitness > worstFitness){
            worstFitness = temp -> fitness ;
        }
        else if(temp->fitness < bestFitness) {
            bestFitness = temp -> fitness ;
        }
        temp = temp -> nextChromosome ;
    }
}

void rankCalculation() {
    struct Chromosome *temp = headChromosome ;
    while(temp != NULL) {
        if(bestFitness == worstFitness) {
            temp -> rank = 0 ;
        }
        else {
            temp -> rank = (double)(temp->fitness - bestFitness) / (double)(worstFitness-bestFitness) ;
        }
        temp = temp -> nextChromosome ;
    }
}

void swap(struct Chromosome *temp1,struct Chromosome *temp2) {
    int tempFitness ;
    tempFitness = temp1->fitness ;
    temp1 -> fitness = temp2 -> fitness ;
    temp2 -> fitness = tempFitness ;

    struct Gene *tempGene ;
    tempGene = temp1 ->headGene ;
    temp1 ->headGene = temp2 ->headGene ;
    temp2 -> headGene = tempGene ;
}

void sortPopulation() {
    int i ;
    do{
        i = 0 ;
        struct Chromosome *temp1=NULL,*temp2= headChromosome ;
        while(temp2->nextChromosome != temp1) {
            if(temp2->fitness > temp2->nextChromosome->fitness) {
                swap(temp2,temp2->nextChromosome) ;
                i = 1 ;
            }
            temp2 = temp2 -> nextChromosome ;
        }
        temp1 = temp2 ;
    }while(i) ;
}

void Crossover(int chromosomeIndex1,int chromosomeIndex2,int xoverParam1,int xoverParam2,int mutateParam) {
    int i ;
    struct Chromosome *temp1 = headChromosome ;
    struct Chromosome *temp2 = headChromosome;
    /* set selected chromosomes to temp1 and temp2 */
    for(i=0;i<chromosomeIndex1-1;i++) {
        temp1 = temp1 ->nextChromosome ;
    }
    for(i=0;i<chromosomeIndex2-1;i++) {
        temp2 = temp2 -> nextChromosome ;
    }
    struct Gene *tempGene1  = temp1->headGene ;
    struct Gene *tempGene2 = temp2 -> headGene ;
    /* set selected genes to tempGene1 and tempGene2 */
    for(i=0;i<xoverParam1-1;i++) {
        tempGene1 = tempGene1->nextGene ;
        tempGene2 = tempGene2 -> nextGene ;
    }
    /* crossover happens here */
    for(i=0;i<xoverParam2-xoverParam1+1;i++) {
        int tempData = tempGene1->data ;
        tempGene1->data = tempGene2->data ;
        tempGene2->data = tempData ;
        tempGene1 = tempGene1 ->nextGene;
        tempGene2 = tempGene2 -> nextGene ;
    }

    tempGene1 = temp1->headGene ;
    tempGene2 = temp2 -> headGene ;
    for(i=0;i<mutateParam-1;i++) {
        tempGene1 = tempGene1->nextGene ;
        tempGene2 = tempGene2->nextGene ;
    }
    tempGene1->data= (tempGene1->data == 0) ? 1 : 0 ;
    tempGene2->data = (tempGene2->data==0) ? 1 : 0 ;
}

void Selection(char *selectionLine,char *xoverLine,char *mutateLine,int pop_size) {
    int mutateParam = atoi(mutateLine) ;
    char *xoverToken = strtok(xoverLine,":\n\r") ;
    int xoverParam1 = atoi(xoverToken) ;
    xoverToken = strtok(NULL,":\n\r") ;
    int xoverParam2 = atoi(xoverToken) ;
    char *selectionToken = strtok(selectionLine," :\n\r") ;
    int selectionArray[pop_size] ;
    int i=0;
    while(selectionToken != NULL) {
        int chromosomeNumber = atoi(selectionToken) ;
        selectionArray[i] = chromosomeNumber ;
        i++ ;
        selectionToken = strtok(NULL," :\n\r") ;
    }
    for(i=0;i<pop_size;i+=2) {
        Crossover(selectionArray[i],selectionArray[i+1],xoverParam1,xoverParam2,mutateParam) ;
    }
}

void Display(int generation,int *bestGenes,int prob_size) {
    printf("GENERATION: %d\n",generation) ;
    struct Chromosome *temp = headChromosome ;
    while(temp != NULL) {
        struct Gene *tempGene = temp -> headGene ;
        while(tempGene != NULL) {
            if(tempGene->nextGene != NULL) {
                printf("%d:",tempGene->data) ;
                tempGene = tempGene -> nextGene ;
            }
            else {
                printf("%d",tempGene->data) ;
                tempGene = tempGene->nextGene ;
            }
        }
        printf(" -> %d\n",temp->fitness) ;
        temp = temp ->nextChromosome ;
    }
    printf("Best chromosome found so far: ") ;
    int i = 0 ;
    for(;i<prob_size;i++) {
        if(i != prob_size-1) {
            printf("%d:",bestGenes[i]) ;
        }
        else {
            printf("%d",bestGenes[i]) ;
        }
    }
    printf(" -> %d\n",bestChromosome->fitness) ;
}

void copyBestGene(int *bestGenes,int prob_size) {
    struct Gene *temp = headChromosome->headGene ;
    int i = 0 ;
    while(temp != NULL) {
        bestGenes[i] = temp ->data ;
        i++ ;
        temp = temp ->nextGene ;
    }
}

int main(int argc,char *argv[]) {
    FILE *fp ;
    fp = fopen("population","r") ;

    int prob_size = atoi(argv[1]) ;  /* number of genes in any chromosome. */
    int pop_size = atoi(argv[2]) ;   /* total number of chromosome in population. */
    int max_gen = atoi(argv[3]) ;    /* total number of iteration to terminate evolutionary process. */
    int generation = 0 ;
    headChromosome = NULL ;
    bestChromosome = (struct Chromosome*) malloc(sizeof(struct Chromosome)) ;
    int *bestGenes = malloc(sizeof(int)*prob_size) ;

    /* FILE READING  */
    char *population[pop_size]  ;   /* array for store population file  */
    if(fp == NULL) {
        return 0 ;
    }
    size_t len = 0;
    int  i ;

    for(i=0;i<pop_size;i++) {
        population[i] = NULL ;
        len = 0 ;
        getline(&population[i],&len,fp) ;
    }
    fclose(fp) ;

    char *selection[max_gen] ;    /* array for store selection file */
    fp = fopen("selection","r") ;
    if(fp == NULL) {
        return 0 ;
    }
    for(i=0;i<max_gen;i++) {
        selection[i] = NULL ;
        len = 0 ;
        getline(&selection[i],&len,fp) ;
    }
    fclose(fp) ;

    char *xover[max_gen] ;      /* array for store xover file  */
    fp= fopen("xover","r") ;
    if(fp == NULL) {
        return 0 ;
    }
    for(i=0;i<max_gen;i++) {
        xover[i] = NULL ;
        len = 0 ;
        getline(&xover[i],&len,fp) ;
    }
    fclose(fp) ;

    char *mutate[max_gen] ;     /* array for mutate file  */
    fp = fopen("mutate","r") ;
    if(fp == NULL) {
        return 0 ;
    }
    for(i=0;i<max_gen;i++) {
        mutate[i] = NULL ;
        len = 0 ;
        getline(&mutate[i],&len,fp) ;
    }
    fclose(fp) ;

    /* Population initialization */
    for(i=0;i<pop_size;i++) {
        createChromosome(population[i]) ;
    }
    fitnessCalculation(prob_size) ;
    bestFitness = headChromosome ->fitness ;
    worstFitness = headChromosome ->fitness ;
    findBestWorstFitness() ;
    rankCalculation() ;
    sortPopulation();
    bestChromosome->fitness = headChromosome -> fitness ;
    copyBestGene(bestGenes,prob_size) ;
    Display(generation,bestGenes,prob_size) ;
    generation++ ;
    for(i=0;i<max_gen;i++) {
        Selection(selection[i],xover[i],mutate[i],pop_size) ;
        fitnessCalculation(prob_size) ;
        bestFitness = headChromosome ->fitness ;
        worstFitness = headChromosome ->fitness ;
        findBestWorstFitness();
        rankCalculation();
        sortPopulation();
        if(bestFitness < bestChromosome->fitness) {
            bestChromosome->fitness = headChromosome -> fitness ;
            copyBestGene(bestGenes,prob_size) ;
        }
        Display(generation,bestGenes,prob_size) ;
        generation++ ;
    }
    return 0;
}
