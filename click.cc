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

enum clickschemetype {CLICK_BF, CLICK_RANGE};
clickschemetype clickscheme = CLICK_BF;

int main(int argc, char* argv[]) {
  char filename[255];
  if (datascheme == DATA_INTERNET2) {
    //strcpy(filename, "fib-internet2.txt");
    strcpy(filename, "fib-i2-nonoverlap.txt");
  } else if (datascheme == DATA_FUNET) {
    strcpy(filename, "fib-funet.txt");
  }

  ifstream ifs(filename);
   prefix p;
   uint32_t tmp, nexthop;
   uint32_t addr, len;
   ofstream ofs("click.txt");


   if (clickscheme == CLICK_BF) {
     ofs << "data::BfData();" << endl;
     ofs << "bfrt:: BfIPLookup(data);" << endl;
     ofs << "cbfrt :: CbfRouteTable(data," << endl;
   } else if (clickscheme == CLICK_RANGE) {
     ofs << "rt :: RangeIPLookup(" << endl;
   }
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
   int index = 0;
   if (datascheme == DATA_INTERNET2) {
     char str[IPLEN];
     while (ifs >> p >> str) {
       nexthop = str2ip(str);
       if (nhmap.find(nexthop) == nhmap.end()) {
         nhmap[nexthop] = index;
         strcpy(nhstr[index], str);
         index ++;
       }
       pmap[p] = nexthop;
       //ofs << p << " " << str << " " << nhmap[nexthop] << "," << endl;
       ofs << p << " " << nhmap[nexthop] << "," << endl;
     }
   }

   ofs << ");" << endl;
   
   if (clickscheme == CLICK_BF) {
     ofs << "InfiniteSource() -> GetIPAddress(0) -> SetFileIPAddress()->bfrt;" << endl;
   } else if (clickscheme == CLICK_RANGE) {
     ofs << "InfiniteSource() -> GetIPAddress(0) -> SetFileIPAddress()->rt;" << endl;
   }
   for (int i = 0; i < index; i ++) {
     if (clickscheme == CLICK_BF) {
       ofs << "bfrt[" << i << "] -> ac" << i << "::AverageCounter() -> Discard;" << endl;
       ofs << "cbfrt[" << i << "] -> Discard;" << endl;
     } else if (clickscheme == CLICK_RANGE) {
       ofs << "rt[" << i << "] -> ac" << i << "::AverageCounter() -> Discard;" << endl;       
     }
   }

}
