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
    strcpy(filename, "fib-internet2.txt");
  } else if (datascheme == DATA_FUNET) {
    strcpy(filename, "fib-funet.txt");
  }

  ifstream ifs(filename);
   prefix p;
   uint32_t tmp, nexthop;
   uint32_t addr, len;
   ofstream ofs("flat.txt");

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
       if (p.len == 24) {
         pfxlist.insert(p);
         ofs << p << " " << str << endl;
       } else otherlist.push_back(p);
     }
   }

   //exit(0);
   
   for (vector<prefix>::iterator mi = otherlist.begin(); mi != otherlist.end(); mi ++) {
     if (mi->len < 24) {
       p.addr = mi->addr;
       p.len = 24;
       //cout << p << endl;
       for (int i = 0; i < (int)(pow(2, 24 - mi->len)); i ++) {
         if (pmap.find(p) == pmap.end()) {
           pfxlist.insert(p);
           pmap[p] = pmap[*mi];
           if (datascheme == DATA_FUNET) {
             ofs << p.addr << " " << (int)p.len << " " << pmap[p] << endl;
           } else if (datascheme == DATA_INTERNET2) {
             ofs << p << " " << nhstr[nhmap[pmap[p]]] << endl;
           }
         }
         p.addr += 0x00000100;
         }
     } else if (mi->len > 24) {
       p.addr = mi->addr & 0xffffff00;
       p.len = 24;
       if (pmap.find(p) == pmap.end()) {
         pfxlist.insert(p);
         pmap[p] = pmap[*mi];
         if (datascheme == DATA_FUNET) {
           ofs << p.addr << " " << (int)p.len << " " << pmap[p] << endl;
         } else if (datascheme == DATA_INTERNET2) {
           ofs << p << " " << nhstr[nhmap[pmap[p]]] << endl;
         }
       }
     }
   }
}
