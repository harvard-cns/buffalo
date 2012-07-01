#include <fstream>
#include <iterator>
#include <algorithm>
#include <map>
#include <set>

using namespace std;

#include "util.h"
#include "BloomFilter.h"
#include "CountingBloomFilter.h"
#include "prefix.h"
#include "etheraddress.h"

int mac;

#define NEXTHOP_COUNT 10
#define TOTAL_TABLE_SIZE 1.5*1024*1024

bloom_filter bf[NEXTHOP_COUNT];
counting_bloom_filter cbf[NEXTHOP_COUNT];

void route_update(EtherAddress eaddr, int inport, int outport) {
  cbf[inport].remove(eaddr);
  cbf[outport].insert(eaddr);
  bf[inport].remove(eaddr);
  bf[outport].insert(eaddr);
}

void update_bf() {
  int i, j;
  for (i = 0; i < NEXTHOP_COUNT; i ++) {
    for (j = 0; j < (int)TOTAL_TABLE_SIZE; j ++) {
      bf[i].hash_table[j%bf[i].table_size] = cbf[i].hash_table[j];
    }
  }
}

int main(int argc, char* argv[]) {
  char filename[255];
  int n = NEXTHOP_COUNT;
  sprintf(filename, "%s", argv[1]);
  ifstream infile(filename);
  int a[1000];
  int i, j;
  int total = 200000;
  for (i = 0; i < n; i ++) {
    infile >> a[i];
    bf[i].bloom_filter_init((int)(TOTAL_TABLE_SIZE/total*a[i]), a[i], 8);
    cbf[i].counting_bloom_filter_init(int(TOTAL_TABLE_SIZE), a[i], 8);
  }
  for (i = 0; i < n; i ++) {
    for (j = 0; j < a[i]; j ++) {
      mac ++;
      EtherAddress eaddr(mac);
      bf[i].insert(mac);
      cbf[i].insert(mac);
    }
  }

  double totaltime = 0;
  double test = 1;
    double start = get_current_time();
    /*  for (i = 0; i < test; i ++) {
    EtherAddress eaddr(1000);
    route_update(eaddr, 0, 1);
    route_update(eaddr, 1, 0);
    }*/
    for (i = 0; i < test*2; i ++) {
      update_bf();
    }
    double end = get_current_time();
    totaltime+= end - start;
  cout << "time " << totaltime/(test*2) << endl;
}
