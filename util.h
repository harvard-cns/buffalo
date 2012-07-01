#ifndef UTIL_H
#define UTIL_H

#include <math.h>
#include <arpa/inet.h>
#include <iostream>
#include <vector>
//#include "etheraddress.h"

enum schemetype {SCHEME_LPM, SCHEME_FLAT};

#define IPLEN 64
#define MAX_HASH_FUNCTION 100
#define MAX_BLOOM_FILTER 100

uint32_t str2ip(const char * str);
char * ip2str(uint32_t ip);

class ShortestPath {
 public:
  double length;
  int next;
};

class Link {
 public:
  int from;
  int to;
  double weight;
};

void calc_shortest_path(ShortestPath * spath, int n, int m, Link * link);

double get_current_time();

#endif //UTIL_H
