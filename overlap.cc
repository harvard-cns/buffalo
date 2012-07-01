#include <fstream>
#include <iterator>
#include <algorithm>
#include <map>
#include <set>

using namespace std;

#include <math.h>
#include "util.h"
#include "prefix.h"


//#define OVERLAP_DEBUG 1

map<prefix, uint32_t> pmap;
set<prefix> outlist;
set<prefix> alllist;
set<prefix> newlist;
vector<prefix> otherlist;

map<uint32_t, int> nhmap;
char nhstr[1000][IPLEN];

enum dataschemetype {DATA_ROUTEVIEW, DATA_FUNET, DATA_INTERNET2};

dataschemetype datascheme = DATA_FUNET;

/*void gen_subtree(prefix & x) {
  prefix p = x;
  
  if (alllist.find(p) == alllist.end()) {
    subtree.insert(p);
    return;
  }

  if (p.len < 32) {
  }
  }*/

int main(int argc, char* argv[]) {
  char filename[255];
  if (datascheme == DATA_FUNET)
    strcpy(filename, "fib-funet.txt");
  else if (datascheme == DATA_INTERNET2) {
    strcpy(filename, "fib-internet2.txt");
  }
  ifstream ifs(filename);
   prefix p;
   uint32_t tmp, nexthop;
   uint32_t addr, len;
   ofstream ofs("overlap.txt");

  //FUNET
   if (datascheme == DATA_FUNET) {
     while (ifs >> addr >> len >> tmp) {
       nexthop = tmp ;
       p.addr = addr;
       p.len = len;
       pmap[p] = nexthop;
       alllist.insert(p);
       /*if (p.len == 32) {
         outlist.insert(p);
         ofs << addr << " " << len << " " << tmp << endl;
       } else
       otherlist.push_back(p); */
     }
   }

   cout << " all list " << alllist.size() << endl;

  //INTERNET2
   if (datascheme == DATA_INTERNET2) {
     char str[IPLEN];
     int index = 0;
     while (ifs >> p >> str) {
       nexthop = str2ip(str);
       if (nhmap.find(nexthop) == nhmap.end()) {
         nhmap[nexthop] = index;
         strcpy(nhstr[index], str);
         index ++;
       }
       pmap[p] = nexthop;
       alllist.insert(p);
     }
   }

   //exit(0);

   for (int j = 1; j < 32; j ++) {
     newlist = alllist;
     for (set<prefix>::iterator mi = alllist.begin(); mi != alllist.end(); mi ++) {
       if (mi->len == j) {
#ifdef OVERLAP_DEBUG
         cout << " root " << *mi << endl;
#endif
    
         bool ifcontain = false;
         for (set<prefix>::iterator ti = newlist.begin(); ti != newlist.end(); ti ++) {
           if (*mi != *ti && mi->contains(*ti)) {
             //cout << *ti << endl;
             ifcontain = true;
             break;
           }
         }
         if (ifcontain) {
           newlist.erase(*mi);
           p.addr = mi->addr & (0xffffffff << (32 - mi->len - 1));
           p.len = mi->len + 1;
           if (newlist.find(p) == newlist.end()) {
             newlist.insert(p);
             pmap[p] = pmap[*mi];
           } 
           
           p.addr = (mi->addr + (0x00000001 << (32 - mi->len - 1))) & (0xffffffff << (32 - mi->len - 1));
           p.len = mi->len + 1;
           if (newlist.find(p) == newlist.end()) {
             newlist.insert(p);
             pmap[p] = pmap[*mi];
           } 
         }
       }
     }
     alllist = newlist;
     cout << " all list numbers after length " << j << " count " << alllist.size() << endl;
   }

   //check overlap
   for (set<prefix>::iterator mi = alllist.begin(); mi != alllist.end(); mi ++) {
     for (set<prefix>::iterator ti = alllist.begin(); ti != alllist.end(); ti ++) {
       if (*ti != *mi && ti->len < mi->len && ti->contains(*mi) ) {
         cout << "error " << *mi << " " << *ti << endl;
       }
     }
   }

   for (set<prefix>::iterator mi = alllist.begin(); mi != alllist.end(); mi ++) {
     if (datascheme == DATA_FUNET) 
       ofs << mi->addr << " " << (int)mi->len << " " << pmap[*mi] << endl;     
     else if (datascheme == DATA_INTERNET2) {
       ofs << *mi << " " << nhstr[nhmap[pmap[*mi]]] << endl;
     }
   }
}
