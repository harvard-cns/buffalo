#ifndef INCLUDE_BLOOM_FILTER_H
#define INCLUDE_BLOOM_FILTER_H

#include "util.h"
#include "prefix.h"
#include "etheraddress.h"

extern schemetype runscheme;

class bloom_filter
{

public:

  /*bloom_filter(long unsigned int tbl_size, long unsigned int elem_size, unsigned int fun_size) {
    bloom_filter_init(tbl_size, elem_size, fun_size);
    }*/

  bloom_filter() {hash_table = NULL;}
  
  void bloom_filter_init(long unsigned int tbl_size, long unsigned int elem_size, unsigned int fun_size) {
    table_size = tbl_size;
    if (table_size == 0) table_size = 1;
    //cout << " bloom_filter_init: table size " << tbl_size << endl;
    element_size = elem_size;
    if (elem_size == 0) {
      function_size = 0;
      hash_table = NULL;
      return;
    }
    function_size = 8;//(long unsigned int)(tbl_size/elem_size * 0.7);//fun_size;
    if (function_size == 0) function_size = 1;
    hash_table = (unsigned char*) malloc(sizeof(unsigned char) * (table_size));
    unsigned int i;
    for(i = 0; i < table_size; ++i) hash_table[i] = 0;
    //cout << "function size " << function_size << endl;
  }


   ~bloom_filter()
   {
     if (hash_table) free(hash_table);
   }

   int myrand() {
     return rand();
   }

   void insert(const EtherAddress & p)
   {
     //cout << "insert p " << p << endl;
     srand((p.addr[1]<<16)+p.addr[2]);
      for(unsigned int i = 0; i < function_size; i++) {
        long unsigned int hash =  (long unsigned int) (rand()/(double)RAND_MAX * table_size);
        //cout << hash << " " << table_size << endl;
        //Cout << hash << endl;
        if (!hash_table[hash]) hash_table[hash] = 1;
        else hash_table[hash] = 2;
      }
   }

   void remove(const EtherAddress & p)
   {
     //cout << "insert p " << p << endl;
     srand((p.addr[1]<<16)+p.addr[2]);
      for(unsigned int i = 0; i < function_size; i++) {
        long unsigned int hash =  (long unsigned int) (rand()/(double)RAND_MAX * table_size);
        //cout << hash << endl;
        if (hash_table[hash]) hash_table[hash] = 0;
        //else hash_table[hash] = 2;
      }
   }


   void insert(const prefix & p)
   {
     //cout << "insert p " << p << endl;
     srand(p.addr);
      for(unsigned int i = 0; i < function_size; i++) {
        long unsigned int hash =  (long unsigned int) (rand()/(double)RAND_MAX * table_size);
        if (!hash_table[hash]) hash_table[hash] = 1;
        else hash_table[hash] = 2;
      }
   }

   int contains(const EtherAddress& p)
   {
     if (!hash_table) return false;
     //cout << "check p " << p << endl;
     srand((p.addr[1]<<16)+p.addr[2]);
      for (unsigned int i = 0; i < function_size; i++)
      {
        long unsigned int hash = (long unsigned int)(myrand()/(double)RAND_MAX * table_size);
         if (hash_table[hash] == 0) {
            return 0;
         } /*else if (hash_table[hash] == 1) {
           return 1;
         }*/
      }
      return 1;
   }

   int contains(const prefix& p)
   {
     if (!hash_table) return false;
     //cout << "check p " << p << endl;
     srand(p.addr);
      for (unsigned int i = 0; i < function_size; i++)
      {
        long unsigned int hash = (long unsigned int)(myrand()/(double)RAND_MAX * table_size);
         if (hash_table[hash] == 0) {
            return 0;
         } /*else if (hash_table[hash] == 1) {
           return 1;
         }*/
      }
      return 1;
   }

   int check() {
     for (unsigned int i = 0; i < table_size; i ++) {
       if (hash_table[i] > 1) cout << i;
     }
     cout << endl;
     return 0;
   }

public:

   unsigned char*               hash_table;
   long unsigned int                 table_size; //m bits array
   long unsigned int                 element_size; //n elements
   unsigned int                 function_size; //k functions

};

#endif
