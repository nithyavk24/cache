#ifndef MEMORY_H_
#define MEMORY_H_

/*
  This file contains class interfaces, simple constructors,
and simple destructors.  It also defines some global constants
for use in the classes and main program.
*/

//================
//==  Global vars
//================
extern unsigned int clockX;
extern unsigned int numMisses;
extern unsigned int miss;
extern unsigned int error;
void printCacheOrg(int org);


//=================
//==  Constants
//=================
#define WORDS_PER_BLOCK 4
#define BLOCKS_IN_MEMORY 512
#define BLOCKS_IN_CACHE 8

//-------------------------------------


//=================
//==  Block
//=================
class Block{
public:
    Block(); //constructor
    ~Block();//destructor
    int isEmpty();


    int words[WORDS_PER_BLOCK];
    int data[WORDS_PER_BLOCK];
    int last_used;
    int valid;
    int tag;
};//class block
//-------------------------------------


//=================
//==  MainMem
//=================
class MainMem{
private:
    Block blocks[BLOCKS_IN_MEMORY];
public:
    MainMem();  //constructor
    ~MainMem(){ }  //destructor
    int getData(int);
    int putData(int, int);
    int resolveFault(int);
};//class MainMem
//-------------------------------------


//=================
//==  Cache
//=================
class Cache {
private:
    Block cblocks[BLOCKS_IN_CACHE];
    MainMem MainMemory;


public:
    Cache();  //constructor
    ~Cache(){ }  //destructor
    int getData(int);
    int putData(int,int);
    int resolveMiss(int);
    int getOldestBlock();
    void setBlockUsage(int);
    int showCacheBlock0(int);
    int showCacheBlock1(int);
};//class Cache
//-------------------------------------


class Memory
{
private:
    Cache myCache;

public:

    Memory ();                                   // constructor

    int  getData ( int address );                // load
    void putData ( int address, int data );     // store


    unsigned int  getClock ();                   // get current timer value
    void setClock ( unsigned int newtime );      // set new timer value
    void resetClock();                           // set clock to 0

    unsigned int  getNumCacheMisses ();               // get number of cache misses
    void setNumCacheMisses ( unsigned int newval );   // record number of cache misses
    void showCacheAddress (int type);								// show the cache contents
};

#endif
