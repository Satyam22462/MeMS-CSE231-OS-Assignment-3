/*
All the main functions with respect to the MeMS are inplemented here
read the function discription for more details

NOTE: DO NOT CHANGE THE NAME OR SIGNATURE OF FUNCTIONS ALREADY PROVIDED
you are only allowed to implement the functions 
you can also make additional helper functions a you wish

REFER DOCUMENTATION FOR MORE DETAILS ON FUNSTIONS AND THEIR FUNCTIONALITY
*/
// add other headers as required
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <stdint.h>

/*
Use this macro where ever you need PAGE_SIZE.
As PAGESIZE can differ system to system we should have flexibility to modify this 
macro to make the output of all system same and conduct a fair evaluation. 
*/

#define PAGE_SIZE 4096

typedef struct MemorySegment {
    size_t size;
    void*addr;
    int type; // 0 for HOLE, 1 for PROCESS
    struct MemorySegment* next;
    struct MemorySegment* prev;
    long int starting_virtual_address;
    long int ending_virtual_address;
    void*virtual;
} MS;

typedef struct MainNode {
    void*memory;
    struct MemorySegment* sub_chain;
    struct MainNode* down;
    struct MainNode* up;
    long int main_starting_address;
    long int main_ending_address; 
} MN;
MN * head ;
int yo;
long int mems_virtual_address;
/*
Initializes all the required parameters for the MeMS system. The main parameters to be initialized are:
1. the head of the free list i.e. the pointer that points to the head of the free list
2. the starting MeMS virtual address from which the heap in our MeMS virtual address space will start.
3. any other global variable that you want for the MeMS implementation can be initialized here.
Input Parameter: Nothing
Returns: Nothing
*/
void mems_init(){
    head = NULL;
    yo=0;
    mems_virtual_address=1000;
}
/*
This function will be called at the end of the MeMS system and its main job is to unmap the 
allocated memory using the munmap system call.
Input Parameter: Nothing
Returns: Nothing
*/
void mems_finish(){
    MN * headNode = head;
    MN*previous=NULL;
    while(headNode){
        munmap(headNode->memory, ((headNode->main_ending_address-headNode->main_starting_address)+1));
        previous=headNode;
        headNode=headNode->down;
        munmap(previous,sizeof(struct MainNode));
    }
    
}
/*
Allocates memory of the specified size by reusing a segment from the free list if 
a sufficiently large segment is available. 

Else, uses the mmap system call to allocate more memory on the heap and updates 
the free list accordingly.

Note that while mapping using mmap do not forget to reuse the unused space from mapping
by adding it to the free list.
Parameter: The size of the memory the user program wants
Returns: MeMS Virtual address (that is created by MeMS)
*/ 

void *mems_malloc(size_t size){
    MN *previousNode = NULL;
    MN * headNode = head;
    while (headNode)
    {
        if(headNode->sub_chain){
            MS *segment = headNode->sub_chain;
            while (segment){
                if(segment->type==0 && segment->size>=size){
                    if(segment->size>size){
                        MS *hole = mmap(NULL, sizeof(MS), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                        if (hole == MAP_FAILED) {
                            perror("mmap");
                            return NULL;
                        }
                        hole->size = segment->size-size;
                        hole->addr = (char*) segment->addr + size;
                        hole->type = 0;
                        hole->prev = segment;
                        hole->next= segment->next;
                        segment->size = size;
                        segment->type = 1;
                        // printf("a: %lu\n", (unsigned long)segment->addr);
                        // printf("b: %lu\n", (unsigned long)hole->addr);
                        if(segment->next){
                            segment->next->prev = hole;
                        }
                        segment->next = hole;
                        
                        
                        segment->ending_virtual_address=segment->starting_virtual_address+segment->size-1;
                        // segment->virtual=(void*)(intptr_t)segment->starting_virtual_address;
                        hole->starting_virtual_address=segment->ending_virtual_address+1;
                        hole->virtual=(void*)(intptr_t)hole->starting_virtual_address;
                        hole->ending_virtual_address=hole->starting_virtual_address+hole->size-1;
                        
                    }
                    else if (segment->type==0 && segment->size==size)
                    {
                        segment->type = 1;
                    }
                    return (void *)(segment->virtual);
                    
                }
                segment = segment->next;
            }
        }
        previousNode = headNode;
        headNode = headNode->down;
    }
    
    int i=1;
    while(size>(i*PAGE_SIZE)){
        i++;
    }
    size_t page_aligned_size = i*PAGE_SIZE;
    MN *m1 = mmap(NULL, sizeof(MN), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (m1 == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }
    m1->memory=mmap(NULL, page_aligned_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (m1->memory == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }
    m1->down=NULL;
    m1->up=previousNode;
    m1->sub_chain=NULL;
    m1->main_starting_address=mems_virtual_address;
    m1->main_ending_address=m1->main_starting_address+(page_aligned_size-1);
    if(previousNode){
        previousNode->down=m1;
    }
    // printf("MAIN NODE %lu\n", (unsigned long)m1->memory);
    if(yo == 0){
        head = m1;
        yo = 1;
    }
    MS *ms1 = mmap(NULL, sizeof(MS), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ms1 == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }
    ms1->size=size;
    ms1->addr=m1->memory;
    ms1->type=1;
    ms1->next=NULL;
    ms1->prev=NULL;
    ms1->starting_virtual_address=mems_virtual_address;
    
    ms1->ending_virtual_address=ms1->starting_virtual_address+ms1->size-1;
    mems_virtual_address=ms1->starting_virtual_address+ms1->size;
    ms1->virtual=(void*)(intptr_t)ms1->starting_virtual_address;
    
    m1->sub_chain=ms1;
    // printf("FIRST NODE %lu\n",(unsigned long)ms1->addr);
    MS *ms2 = mmap(NULL, sizeof(MS), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ms2 == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }
    ms2->size=page_aligned_size-size;
    ms2->addr=(char*)ms1->addr+size;
    ms2->next=NULL;
    ms2->prev=ms1;
    ms2->type=0;
    ms2->starting_virtual_address=mems_virtual_address;
    mems_virtual_address=ms2->starting_virtual_address+ms2->size;
    ms2->ending_virtual_address=ms2->starting_virtual_address+ms2->size-1;
    ms2->virtual=(void*)(intptr_t)ms2->starting_virtual_address;


    ms1->next=ms2;
    // printf("SECOND NODE %lu\n", (unsigned long)ms2->addr);
    return (void*)(m1->sub_chain->virtual);
}
/*
this function print the stats of the MeMS system like
1. How many pages are utilised by using the mems_malloc
2. how much memory is unused i.e. the memory that is in freelist and is not used.
3. It also prints details about each node in the main chain and each segment (PROCESS or HOLE) in the sub-chain.
Parameter: Nothing
Returns: Nothing but should print the necessary information on STDOUT
*/
void mems_print_stats(){
    printf("---------MeMS SYSTEM STATS---------\n");
    MN * headNode = head;
    
    long int SPACE_UNSED=0;
    int MAIN_CHAIN_LENGTH=0;
    int arr[100];
    int i=0;

    while(headNode){
        printf("MAIN[%ld:%ld]-> ",headNode->main_starting_address,headNode->main_ending_address);
        int count=0;
        if(headNode->sub_chain){
            MS *segment = headNode->sub_chain;
            while(segment){
                if(segment->type==1){
                    printf("P[%ld:%ld] <-> ",segment->starting_virtual_address,segment->ending_virtual_address);
                    count++;

                }
                else if(segment->type==0){
                    printf("H[%ld:%ld] <-> ",segment->starting_virtual_address,segment->ending_virtual_address);
                    count++;
                    SPACE_UNSED=SPACE_UNSED+(segment->ending_virtual_address-segment->starting_virtual_address)+1;
                }
                
                segment=segment->next;
            }
            printf("NULL");
            

        }
        arr[i]=count;
        i++;
        MAIN_CHAIN_LENGTH++;
        headNode=headNode->down;
        printf("\n");
    }
    printf("Pages used\t%ld\n", mems_virtual_address/4096);
    printf("Space unused\t%ld\n",SPACE_UNSED);
    printf("Main chain length\t%d\n",MAIN_CHAIN_LENGTH);
    printf("Sub-chain length array:  [");
    int m=0;
    while(arr[m]!='\0'){
        printf("%d,",arr[m]);
        m++;
    }
    printf("]\n");

}

/*
Returns the MeMS physical address mapped to ptr ( ptr is MeMS virtual address).
Parameter: MeMS Virtual address (that is created by MeMS)
Returns: MeMS physical address mapped to the passed ptr (MeMS virtual address).
*/
void* mems_get(void*v_ptr){
    long int retrievedValue = (long int)(intptr_t)v_ptr;
    

    MN * headNode = head;
    while(headNode){
        if(headNode->sub_chain){
            MS *segment = headNode->sub_chain;
            while(segment){
                if(segment->starting_virtual_address<=retrievedValue && retrievedValue<=segment->ending_virtual_address){
                    if(segment->type==1){
                        void*mems_physical_address=(char*) segment->addr + (retrievedValue-segment->starting_virtual_address);
                        return (void *)(mems_physical_address);
                    }
                    else{
                        printf("ILLEGAL MEMORY ACCESS\n");
                        return NULL;
                    }
                }
                segment=segment->next;
            }

        }
        headNode=headNode->down;
    }
    printf("ILLEGAL MEMORY ACCESS\n");
    return NULL;  
}
/*
this function free up the memory pointed by our virtual_address and add it to the free list
Parameter: MeMS Virtual address (that is created by MeMS) 
Returns: nothing
*/
void mems_free(void *v_ptr){
    long int retrievedValue = (long int)(intptr_t)v_ptr;
    MN * headNode = head;
    while(headNode){
        if(headNode->sub_chain){
            MS *segment = headNode->sub_chain;
            while(segment){
                if(segment->starting_virtual_address==retrievedValue){
                    if(segment->type==1){
                        segment->type=0;
                        while(segment->next && segment->next->type==0){
                            segment->size=segment->size+segment->next->size;
                            segment->ending_virtual_address=segment->next->ending_virtual_address;
                            if(segment->next->next){
                                segment->next->next->prev=segment;
                                segment->next=segment->next->next;
                            }
                            else{
                                segment->next=segment->next->next;
                            }


                        }
                        while(segment->prev && segment->prev->type==0){
                            segment->size=segment->size+segment->prev->size;
                            segment->starting_virtual_address=segment->prev->starting_virtual_address;
                            segment->addr=segment->prev->addr;
                            segment->virtual=(void*)(intptr_t)segment->starting_virtual_address;
                            if(segment->prev->prev){
                                segment->prev->prev->next=segment;
                                segment->prev=segment->prev->prev;
                            }
                            else{
                                headNode->sub_chain=segment;
                                segment->prev=segment->prev->prev;
                            }
                        }

                        return;
                    }
                    else{
                        printf("IT IS ALREADY FREE\n");
                        return;
                        
                    }
                }
                segment=segment->next;
            }

        }
        headNode=headNode->down;
    }
    printf("ILLEGAL MEMORY ACCESS\n");
    

}
