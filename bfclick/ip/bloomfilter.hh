#ifndef INCLUDE_BLOOM_FILTER_H
#define INCLUDE_BLOOM_FILTER_H

#include "click/etheraddress.hh"
#include "util.hh"
//#include "etheraddress.hh"

#define CBF_SIZE 40*1024*10 //2^20
#define BF_SIZE 100 //1000//1048576
#define MAX_ELEM_SIZE 86940
#define BITS_PER_ELEM 32

#define PFXLEN_COUNT 32
#define MAX_PORT_COUNT 10
#define MAX_BF_COUNT 10
#define MAX_CBF_TABLE_SIZE 1024*68

#define BF_DEBUG 1

#define CHANGE_THRESHOLD 1000

static int esize[10] = {141157,
                    17844,
                    10523,
                    10478,
                    6268,
                    4895,
                    2974,
                    2196,
                    1967,
                    1698};
#define TOTAL_MEMORY 1024*1024

const unsigned char bit_mask[8] = {
  0x01, //00000001
  0x02, //00000010
  0x04, //00000100
  0x08, //00001000
  0x10, //00010000
  0x20, //00100000
  0x40, //01000000
  0x80  //10000000
};

const int char_size= 8;

static int aseed[30] = {4, 55, 27, 98, 34, 12, 35, 90, 21, 44, 67, 19, 3, 93, 49, 83, 64, 137, 38, 28, 39, 34, 48, 56, 52, 24, 58, 38, 23, 40};
static int bseed[30] = {27, 98, 34, 12, 35, 90, 21, 44, 67, 19, 3, 93, 49, 83, 64, 137, 38, 28, 39, 34, 48, 56, 52, 24, 58, 38, 23, 40, 43, 63};

class counting_bloom_filter
{

public:

  counting_bloom_filter() {
    //cbf_count = 0;
    function_size = 23;
    table_size = MAX_CBF_TABLE_SIZE;
    //max_port = 0;
  }

   ~counting_bloom_filter()   {
    unsigned int i;
    for (i = 0; i < table_size; i ++) {
     if (hash_table[i]) delete hash_table[i];
    }
   }
  
  void initialize() { //long unsigned int tbl_size, long unsigned int elem_size) {
    unsigned int i;
    for (i = 0; i < table_size; i ++) {
      hash_table[i] = new unsigned char[MAX_BF_COUNT*function_size];
      memset(hash_table[i], 0, sizeof (unsigned char) * MAX_BF_COUNT*function_size);
    }
    #ifdef BF_DEBUG
    click_chatter("CBF initialize end \n");
#endif
  }
  
  int remove(const IPAddress &, int ) {//const IPAddress & p, int prefix_len) {
    /*   unsigned int count[MAX_BF_COUNT];
    memset(count, 0, MAX_BF_COUNT*sizeof(int));
    BF_SRANDOM(p.addr());
    int port;
    int hit = 0;
    int hitindex = 0;
    float * hasharray;
    unsigned int i;
    hasharray = new float[max_function_size];
    for(i = 0; i < max_function_size; i++) {
      float hashtmp =  (long unsigned int) (BF_RANDOM()/(float)CLICK_RAND_MAX);
      hasharray[i] = hashtmp;
      for (port = 0; port < max_port; port ++) {
        int index = port * PFXLEN_COUNT + prefix_len;
        if (i >= function_size[index]) continue;
        long unsigned  int hashvalue = (long unsigned int) (hashtmp/(float)table_size[index]);
        if (!hash_table[index][hashvalue]) {
          count[port] ++;
          if (count[port] >= function_size[index]) {
            hit ++;
            hitindex = index;
          }
        }
      }    
    }
    if (hit > 1) {
      //perror("cbf remove");
      return -1;
    }
    for (i = 0; i < function_size[hitindex]; i ++) {
      hash_table[hitindex][(long unsigned int)(hasharray[i]/(float)table_size[hitindex])] --;
    }
    element_size[hitindex] --;
    return hitindex / PFXLEN_COUNT; */
    return 0;
  }

public:
  //int cbf_count;
  //int max_port;
  //int changes;
  unsigned int function_size;
  long unsigned int table_size;
  unsigned char * hash_table[MAX_CBF_TABLE_SIZE];
  //unsigned char*               hash_table[MAX_BF_COUNT];
  //long unsigned int                 table_size[MAX_BF_COUNT]; //m bits array
  //long unsigned int                 element_size[MAX_BF_COUNT]; //n elements
  //unsigned int                 function_size[MAX_BF_COUNT]; //k functions
  //unsigned int max_function_size;
};

/*
class counting_bloom_filter
{

public:

  counting_bloom_filter() {
    cbf_count = 0;
    max_function_size = 0;
    int i;
    for (i = 0; i < MAX_BF_COUNT; i ++) {
      hash_table[i] = NULL;
    }
    max_port = 0;
  }

   ~counting_bloom_filter()   {
    int i;
    for (i = 0; i < MAX_BF_COUNT; i ++) {
     if (hash_table[i]) delete hash_table[i];
    }
   }
  
  void initialize() { //long unsigned int tbl_size, long unsigned int elem_size) {
    for (int j= 0; j < MAX_BF_COUNT; j ++) {
      table_size[j] = CBF_SIZE;
      element_size[j] = 0;
      function_size[j] = (long unsigned int)(BITS_PER_ELEM * 0.7);//fun_size;
      if (function_size[j] == 0) function_size[j] = 1;
      if (function_size[j] > max_function_size) max_function_size = function_size[j];
      hash_table[j] = new unsigned char[table_size[j]];
      unsigned int i;
      for(i = 0; i < table_size[j]; ++i) hash_table[j][i] = 0;
    }
    #ifdef BF_DEBUG
    click_chatter("CBF initialize end \n");
#endif
  }
  
  int remove(const IPAddress &p, int prefix_len) {
    unsigned int count[MAX_BF_COUNT];
    memset(count, 0, MAX_BF_COUNT*sizeof(int));
    BF_SRANDOM(p.addr());
    int port;
    int hit = 0;
    int hitindex = 0;
    float * hasharray;
    unsigned int i;
    hasharray = new float[max_function_size];
    for(i = 0; i < max_function_size; i++) {
      float hashtmp =  (long unsigned int) (BF_RANDOM()/(float)CLICK_RAND_MAX);
      hasharray[i] = hashtmp;
      for (port = 0; port < max_port; port ++) {
        int index = port * PFXLEN_COUNT + prefix_len;
        if (i >= function_size[index]) continue;
        long unsigned  int hashvalue = (long unsigned int) (hashtmp/(float)table_size[index]);
        if (!hash_table[index][hashvalue]) {
          count[port] ++;
          if (count[port] >= function_size[index]) {
            hit ++;
            hitindex = index;
          }
        }
      }    
    }
    if (hit > 1) {
      //perror("cbf remove");
      return -1;
    }
    for (i = 0; i < function_size[hitindex]; i ++) {
      hash_table[hitindex][(long unsigned int)(hasharray[i]/(float)table_size[hitindex])] --;
    }
    element_size[hitindex] --;
    return hitindex / PFXLEN_COUNT; 
  }

  void printsize() {
    for (int i = 0; i < cbf_count; i ++) {
      click_chatter(" i %d size %d \n", i, table_size[i]);
    }
  }

public:
  int cbf_count;
  int max_port;
  int changes;
  unsigned char*               hash_table[MAX_BF_COUNT];
  long unsigned int                 table_size[MAX_BF_COUNT]; //m bits array
  long unsigned int                 element_size[MAX_BF_COUNT]; //n elements
  unsigned int                 function_size[MAX_BF_COUNT]; //k functions
  unsigned int max_function_size;
};
*/


class bloom_filter {
public:

  /*bloom_filter(long unsigned int tbl_size, long unsigned int elem_size, unsigned int fun_size) {
    bloom_filter_init(tbl_size, elem_size, fun_size);
    }*/

  bloom_filter() {
    bf_count = 0;
    max_function_size= 0;
    int i;
    for (i = 0; i < MAX_BF_COUNT; i ++) {
      hash_table[i] = NULL;
    }
  }
  
  void initialize() {
    for (int j= 0; j < MAX_BF_COUNT; j ++) {
      table_size[j] = esize[j]*TOTAL_MEMORY/200000;
#ifdef BF_DEBUG
      click_chatter("size %d %d %u", j, esize[j], table_size[j]);
#endif
      element_size[j] = 0;//esize[j];
      function_size[j] = 8;//fsize[j];
      if (function_size[j] > max_function_size) max_function_size = function_size[j];
      hash_table[j] = new unsigned char[table_size[j]/8];
      unsigned int i;
      for(i = 0; i < BF_SIZE/8; ++i) hash_table[j][i] = 0;
    }
  }

   ~bloom_filter() {
    int i;
    for (i = 0; i < MAX_BF_COUNT; i ++) {
     if (hash_table[i]) delete hash_table[i];
    }
   }

  /*  void swapin(counting_bloom_filter &cbf, int pfxlen, int port) {
    bf_count = cbf.cbf_count;
    max_port = cbf.max_port;
    unsigned int i;
    int index = port*PFXLEN_COUNT+pfxlen;
    table_size[index] = cbf.element_size[index] * BITS_PER_ELEM;
    element_size[index] = cbf.element_size[index];
    function_size[index] = cbf.function_size[index];
    if (function_size[index] > max_function_size) max_function_size = function_size[index];
    for (i = 0; i < table_size[index]; i ++) {
      //table_size
      }
      }*/

  int lookup(const IPAddress&) {
    /* unsigned int count[MAX_BF_COUNT];
    memset(count, 0, MAX_BF_COUNT*sizeof(int));
    int port;
    int hit = 0;
    int hitport = -1;
    float * hasharray;
    unsigned int i;
    int index;
    int pfxlen;
    IPAddress p = inputp;
    hasharray = new float[max_function_size];
    //click_chatter("bf lookup: max func_size %d", max_function_size);
    for (pfxlen = 32; pfxlen > 0; pfxlen --) {
      //if (pfxlen != 24) continue;
      uint32_t addr = inputp.addr();
      addr = addr << (32 - pfxlen) >> (32-pfxlen);
      //click_chatter("inputp %d, mask pfx %d", inputp.addr(), addr);                   
      p = IPAddress(addr);
      //p = inputp.make_prefix(pfxlen);
      BF_SRANDOM(p.addr());
      //click_chatter("address %d %s pfxlen %d", p.addr(), p.unparse().c_str(), pfxlen);
      for(i = 0; i < max_function_size; i++) {
        float rand = 0; //BF_RANDOM();
        float hashtmp =  rand/(float)CLICK_RAND_MAX;
        //click_chatter("hashtmp %lf max_port %d", hashtmp, max_port);
        hasharray[i] = hashtmp;
        for (port = 0; port < max_port; port ++) {
          index = port * PFXLEN_COUNT + pfxlen;
          if (i >= function_size[index]) continue;
          long unsigned  int hashvalue = (long unsigned int) (hashtmp*(float)table_size[index]);
          if (hash_table[index][hashvalue/char_size] & bit_mask[hashvalue % char_size]) {
            //click_chatter("hit");
            count[index] ++;
          }
        }
      }    
    }

    //print_hashtable(24, 0);
    //print_hashtable(25, 0);

    for (port = 0; port < max_port; port ++) {
      for (pfxlen = 32; pfxlen > 0; pfxlen --) {
        index = port*PFXLEN_COUNT + pfxlen;
        if (count[index] == function_size[index]) {
          hit ++;
          hitport = port;
          break;
        }
      } 
    }
    if (hit > 1) {
      click_chatter("bf lookup: multiple hits\n");
      return -1;
    }
    if (hitport < 0) {
      //click_chatter("error! exit");
    }
    //click_chatter("bloom filter lookup hitport %d", hitport);
    return hitport;
    */
    return -1;
   }

  int etherlookup(const EtherAddress& inputp) {
    unsigned int count[MAX_BF_COUNT];
    memset(count, 0, MAX_BF_COUNT*sizeof(int));
    int port;
    int hit = 0;
    int hitport = -1;
    int * hasharray;
    unsigned int i;
    int index;
    EtherAddress p(inputp);
    hasharray = new int[max_function_size];
    const uint16_t * pdata = p.sdata();
    int addr  =(pdata[1] << 8) + pdata[2];
    //BF_SRANDOM(addr);
    for(i = 0; i < max_function_size; i++) {
      long unsigned int rand = aseed[i]*addr+bseed[i];
      //int hashtmp =  rand;
      //click_chatter("hashtmp %lf max_port %d", hashtmp, max_port);
      hasharray[i] = rand;
      for (port = 0; port < bf_count; port ++) {
	index = port;
	if (i >= function_size[index]) continue;
	long unsigned  int hashvalue = (long unsigned int) (rand % table_size[index]);
        //click_chatter("%d %d %d %d", rand, index, table_size[index], hashvalue);
	if (hash_table[index][hashvalue/char_size] & bit_mask[hashvalue % char_size]) {
	  //click_chatter("hit");
	  count[index] ++;
	}
      }
    }

    for (port = 0; port < bf_count; port ++) {
      index = port;
      if (count[index] == function_size[index]) {
	hit ++;
	hitport = port;
	break;
      }
    }
    if (hit > 1) {
      click_chatter("bf lookup: multiple hits\n");
      return -1;
    }
    if (hitport < 0) {
      //click_chatter("error! exit");
    }
    //click_chatter("bloom filter lookup hitport %d", hitport);
    return hitport;
   }

  void etherinsert(EtherAddress &p, int port) {
         //cout << "insert p " << p << endl;
    int index = port;
    if (port >= max_port) {
      bf_count += 1;
      max_port = port+1;
    }
    const uint16_t * data= p.sdata();
    int addr = (data[1] << 8) + data[2];
     element_size[index]++;
     //int n = element_size[index];
     //click_chatter("table size %d elem size %d func_size %d\n", table_size[index], element_size[index], function_size[index]);
     for(unsigned int i = 0; i < function_size[index]; i++) {
         int rand = aseed[i] * addr +bseed[i];
         //click_chatter("rand %lf %d %d %d", rand, INT_MAX, CLICK_RAND_MAX, table_size[index]);
         long unsigned int hash =  (long unsigned int) (rand);
         //click_chatter("hash %d ", hash);
         
        //update bf
        if (i < function_size[index]) {
            int bfpos= hash % table_size[index];
            //click_chatter("%d %d %u %d", hash, index, table_size[index], bfpos);
            hash_table[index][bfpos/char_size] |= bit_mask[bfpos%char_size];    
        }
      }
  }

  void print_hashtable(int pfxlen, int port) {
    click_chatter("hash table pfxlen %d port %d", pfxlen, port);
    int index = port *PFXLEN_COUNT + pfxlen;
    for (unsigned int i = 0; i < table_size[index]; i ++) {
      if (hash_table[index][i] > 0) click_chatter("%x ", hash_table[index][i]);
    }
    //click_chatter("\n");
  }

public:
  int max_port;
  int bf_count;
  unsigned char*               hash_table[MAX_BF_COUNT];
  long unsigned int                 table_size[MAX_BF_COUNT]; //m bits array
  long unsigned int                 element_size[MAX_BF_COUNT]; //n elements
  unsigned int                 function_size[MAX_BF_COUNT]; //k functions
  unsigned int max_function_size;
  //float total_n;
  //float total_nlnn;
  //float fp;
};

#endif
