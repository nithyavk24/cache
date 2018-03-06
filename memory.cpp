#include "memory.h"
#include <iostream>


using namespace std;
unsigned int clockX;
unsigned int numMisses=0;
unsigned int miss;
unsigned int error;
unsigned int mem_access;
unsigned int org;





void printCacheOrg(int org)
{
    cout<<"Cache Organization: " <<"\n";
    if (org == 0)
        cout<<"FULLY ASSOCIATIVE"<<"\n";
    else if (org == 1)
        cout<<"DIRECT MAPPED"<<"\n";
    else
        cout<<"ERROR: WRONG CACHE ORG"<<"\n";
}


//constructor - initialise all cache blocks to 0
inline Cache::Cache()
{
    int i,j;
    for(i=0;i<BLOCKS_IN_CACHE;i++)
    {
        cblocks[i].last_used=0;
        cblocks[i].valid=0;
        cblocks[i].tag=0;
        for(j=0;j<WORDS_PER_BLOCK;j++)
        {
            cblocks[i].words[j]=0;
            cblocks[i].data[j]=0;
        }
    }
}


//main memory constructor - initialise all data elements to 1
inline MainMem::MainMem()
{
    int i,j;
    for(i=0;i<BLOCKS_IN_MEMORY;i++)
    {
        blocks[i].last_used=0;
        blocks[i].valid=0;
        blocks[i].tag=0;
        for(j=0;j<WORDS_PER_BLOCK;j++)
        {
            blocks[i].words[j]=1;
            blocks[i].data[j]=1;
        }
    }
}


//try to get the data from cache
/* fetching the data from the cache memory location*/
int Memory::getData(int address)
{
    error=0;
    miss=0;                              //initializing
    mem_access=0;
    int data=myCache.getData(address);  //coping the data from the cache loccation
    if(error)
    {
        cout<<"Invalid address. Available addresses - 0,1,2...4096. Data could not be read\n";
        return 0;
    }
    else
    {
        if(miss==1)                     //if data is not found in cache
        {
            setNumCacheMisses(getNumCacheMisses()+1);       //incrementing miss count
            setClock(getClock()+100);                       //increasing clock time
        }
        else
            setClock(getClock()+2);                         //if data found in cache memory, clock time increment 2ns
        return(data);
    }
}


//retrieves a data element from the integer address
int Cache::getData(int address)
{
    int mem_block_num = address/WORDS_PER_BLOCK;          // mapping the address to the multi word cache block
    int wordoffset = address%WORDS_PER_BLOCK;             //  getting single word among the 4 words in the block
    int hit=0;
    int data;                                             //initialization
    int i;
    for(i=0;i<BLOCKS_IN_CACHE;i++)
    {
        if(cblocks[i].valid==1 && cblocks[i].tag==mem_block_num)    //checking for hit condition
        {
            //hit
            hit=1;
            data=cblocks[i].data[wordoffset];                       //accessing data in cache on hit condition
            setBlockUsage(i);                                       //registering the usage of particular cache block
            break;
        }
    }
    if(hit==0)                                                      //on miss
    {
        //miss
        int cache_block_num=resolveMiss(address);                  //getting address and data from memory
        data=cblocks[cache_block_num].data[wordoffset];            //sending the value of data to naive / smart
        setBlockUsage(cache_block_num);                           //registering the block number in which data is stored
    }
    return data;
}


//retrieves an integer element from 'address'
int MainMem::getData(int address)
{
    int i;
    if(address>(BLOCKS_IN_MEMORY*WORDS_PER_BLOCK)||address<0)           //checking if the address is in given range
    {
        resolveFault(address);                                          //if address is out of range sets error to 1
        return 0;
    }
    else
    {
        int mem_block_num=address/WORDS_PER_BLOCK;
        int wordoffset=address%WORDS_PER_BLOCK;
        return blocks[mem_block_num].data[wordoffset];                       //returns main memory address to cache
        return 0;
    }
}



//inserts an integer at address
int MainMem::putData(int address, int data)
{
    if(address>(BLOCKS_IN_MEMORY*WORDS_PER_BLOCK)||address<0)           //checking if the address is in given range
    {
        resolveFault(address);                                          // if address is out of range sets error to 1
        return 0;
    }
    else
    {
        int i;
        int mem_block_num=address/WORDS_PER_BLOCK;
        int wordoffset=address%WORDS_PER_BLOCK;
        blocks[mem_block_num].data[wordoffset]=data;                   // if address match writes data into memory
    }
    return 0;
}



//puts a word(integer) at the desired integer address
int Cache::putData(int address, int data)
{
    int mem_block_num=address/WORDS_PER_BLOCK;
    int wordoffset=address%WORDS_PER_BLOCK;
    int i,j;                                                  //initialization
    int hit=0;
    for(i=0;i<BLOCKS_IN_CACHE;i++)
    {
        if(cblocks[i].valid==1 && cblocks[i].tag==mem_block_num)        //checking if the address is present in cache
        {
            //hit
            hit = 1;                                                    //setting hit to 1 if address is found
            //write to cache
            cblocks[i].data[wordoffset] = data;                         // putting data from naive6 to cache c = a * b
            setBlockUsage(i);                                           // registering the usage of block
            break;
        }
    }

    if(hit==0)                                                          //if data is not found in cache
    {
        //miss
        int cache_block_num=resolveMiss(address);                       //get address from main memory to cache
        //write to cache
        cblocks[cache_block_num].data[wordoffset] = data;                 //copying data from naive/smart to cache
        setBlockUsage(cache_block_num);                                 // registering usage of block
    }

    //write to MainMem
    MainMemory.putData(address,data);                                   //write through operation
    mem_access=1;
    return 0;
}


//try to write data into cache
/*checking whether the address to be written in cache or not*/
void Memory::putData(int address, int data)
{
    error=0;
    miss=0;                                 // initialization
    mem_access=0;
    myCache.putData(address,data);          //putting the data and address in cache
    if(error)
    {
        cout<<"Invalid address. Available addresses - 0,1,2...4096. Data could not be written\n";
    }
    else
    {
        if(miss || mem_access)
        {
            if(miss)
                setNumCacheMisses(getNumCacheMisses()+1); //increment miss number if data is being written in to cache
            setClock(getClock()+100);                        //incremting clock time due to miss
        }
        else
            setClock(getClock()+2);                         //if data is found in cache(hit) increment clock by 2ns
    }
}


//bring the missing block from memory
int Cache::resolveMiss(int address)
{
    //bring data from MainMem
    int cache_block_num;
    int mem_block_num=address/WORDS_PER_BLOCK;
    if(org==0) {                                            // resolving miss for fully associative
        cache_block_num = getOldestBlock();
        //cout<<"fully\n";
    }
    else if(org == 1){                                      // resolving miss for direct mapped
        cache_block_num = mem_block_num % BLOCKS_IN_CACHE;
        //cout<<"direct\n";
    }
    int i;                                                          // initialization
    int start_address=mem_block_num*WORDS_PER_BLOCK;

        for (i = 0; i < WORDS_PER_BLOCK; i++) {
            int data = MainMemory.getData(
                    start_address + i);        //getting address location of the 4 words from main memory
            if (!error) {
                cblocks[cache_block_num].data[i] = data;                  // data is copied from main memory to cache
            }
        }
        if (!error) {
            //write to cache
            cblocks[cache_block_num].valid = 1;                           //setting valid it to 1
            cblocks[cache_block_num].tag = mem_block_num;                 //coping main mameory address to cache loction
            miss = 1;
        }

    return cache_block_num;
}


//returns the cache block number with least value of last_used
int Cache::getOldestBlock()
{
    int i;
    int least_last_used=cblocks[0].last_used;               //initialising the least last used to 0
    int oldest_block_num=0;                                 //oldest block not used in cache set to 0
    for(i=0;i<BLOCKS_IN_CACHE;i++)
    {
        if(cblocks[i].last_used<least_last_used)            //checks which block is used less
        {
            least_last_used=cblocks[i].last_used;           //least last used value updates to lowest value of blocks
            oldest_block_num=i;                             //least used block is the oldest block
        }
    }
    return(oldest_block_num);
}

//constructor
inline Block::Block()
{
}


//destructor
Block::~Block()
{
}


//check if a block is empty by checking its valid bit
int Block::isEmpty()
{
    return(valid==0);
}

unsigned int Memory::getNumCacheMisses()
{
    return numMisses;                                       //number of misses that has occured
}

void Memory::setNumCacheMisses ( unsigned int newval )
{
    numMisses=newval;                                       //updating the incremented value of number of misses
}

void Cache::setBlockUsage(int cache_block_num)
{
    cblocks[cache_block_num].last_used++;
}

///set error flag if there is address error
int MainMem::resolveFault(int address)
{
    error=1;
}

Memory::Memory()
{
}

void Memory::resetClock()
{
    clockX=0;
}

unsigned int Memory::getClock()
{
    return clockX;
}

void Memory::setClock(unsigned int new_time)
{
    clockX=new_time;
}


//display cache block contents
void Memory::showCacheAddress(int type)
{
    int i,t;
    t = type;
    if(t==0) {
        cout << "Cache block No\tcache address\tData\tValid\ttag\tlast_used\n";      // printing the cache values
        for (i = 0; i < BLOCKS_IN_CACHE; i++)
            myCache.showCacheBlock1(i);

    }
    else if(t==1){
        cout << "Cache block No\tcache address\tData\tValid\ttag\n";      // printing the cache values
        for (i = 0; i < BLOCKS_IN_CACHE; i++)
            myCache.showCacheBlock0(i);

    }
}


//display a cache block
int Cache::showCacheBlock1(int cache_block_num)
{
    int i;
    for(i=0;i<WORDS_PER_BLOCK;i++)
    {
        cout<<cache_block_num<<"\t\t"<<&cblocks[cache_block_num].data[i]<<"\t"<<cblocks[cache_block_num].data[i]<<"\t";
        cout<<cblocks[cache_block_num].valid<<"\t"<<cblocks[cache_block_num].tag<<"\t";
        cout<<cblocks[cache_block_num].last_used<<"\n";

    }
    int n,x;
    for(n=0;n<10000;n++){
        x++;
    }
}


int Cache::showCacheBlock0(int cache_block_num)
{
    int i;
    for(i=0;i<WORDS_PER_BLOCK;i++)
    {
        cout<<cache_block_num<<"\t\t"<<&cblocks[cache_block_num].data[i]<<"\t"<<cblocks[cache_block_num].data[i]<<"\t";
        cout<<cblocks[cache_block_num].valid<<"\t"<<cblocks[cache_block_num].tag<<"\n";

    }
    int n,x;
    for(n=0;n<10000;n++){
        x++;
    }
}