#include <fstream>
#include <iterator>
#include <algorithm>
#include <map>
#include <set>

using namespace std;

#include <math.h>
#include "util.h"
#include "prefix.h"

map<prefix, uint32_t> pmap;
set<prefix> pfxlist;
vector<prefix> otherlist;

map<uint32_t, int> nhmap;
char nhstr[1000][IPLEN];

enum dataschemetype {DATA_ROUTEVIEW, DATA_FUNET, DATA_INTERNET2};

dataschemetype datascheme = DATA_INTERNET2;

int main(int argc, char* argv[]) {
  char filename[255];
  if (datascheme == DATA_INTERNET2) {
    //strcpy(filename, "fib-internet2.txt");
    strcpy(filename, "packet-internet2.txt");
  } else if (datascheme == DATA_FUNET) {
    strcpy(filename, "fib-funet.txt");
  }

  ifstream ifs(filename);
   prefix p;
   uint32_t tmp, nexthop;
   uint32_t addr, len;
   ofstream ofs("packets.txt");

  //FUNET
   if (datascheme == DATA_FUNET) {
     while (ifs >> addr >> len >> tmp) {
       nexthop = tmp ;
       p.addr = addr;
       p.len = len;
       pmap[p] = nexthop;
       if (p.len == 24) {
         pfxlist.insert(p);
         ofs << addr << " " << len << " " << tmp << endl;
       } else otherlist.push_back(p);
     }
   }

  //INTERNET2
   int xseed = (unsigned) time(NULL);
   srand(xseed);
   int rv;
   
   if (datascheme == DATA_INTERNET2) {
     //read fib
     char str[IPLEN];
     set<uint32_t> pfx[33];
     strcpy(filename, "fib-i2-nonoverlap.txt");
     ifstream fib_ifs(filename);
     while (fib_ifs >> p >> str) {
       pfx[p.len].insert(p.addr);
     }

     //read packets
     while (ifs >> str) {
       addr = str2ip(str);
       rv = rand();
       uint32_t  offset  = (uint32_t)(rv/(double)RAND_MAX * exp2(11));
       bool hit = false;
       for (int i = 0; i < exp2(11); i ++) {
         bool contains = false;
         uint32_t test = (offset+i) % (uint32_t)exp2(11) + addr;
         for (int j = 0; j < 32; j ++) {
           uint32_t tmp = test >> j << j;
           if (pfx[32-j].find(tmp) != pfx[32-j].end()) {
             contains = true;
             break;
           }
         }
         if (contains) {
           ofs << test << endl;
           hit = true;
           break;
         }
       }
       if (!hit) {
         cout << "error" << endl;
         // ofs << offset +addr << endl;
       }
     }
   }

}
