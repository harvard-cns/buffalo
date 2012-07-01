#include <iostream>
#include <fstream>
#include <set>
#include <map>

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "util.h"
//#include "etheraddress.h"

const int MAXNODE=1700;
const int MAXLINK=250000;
const int MAXINT = 999999;

#define CALC_EXP 1

int fpexp = 3;
double falsepos = 0.001;

enum dataschemetype {DATA_ISP, DATA_CAMPUS, DATA_DC};
dataschemetype datascheme = DATA_ISP;

//map<EtherAddress, int> etherindex;
//int indexstart = 0;

vector<int> neighbors[MAXNODE];
ShortestPath *sp;
int n;

map<int, set<int> > net;

double calc_latency(int src, int dst, int prev, double prob) {
  const double threshold = 1e-10;
  if (src == dst) return 0;
  if (prob < threshold) {
    return sp[src*n+dst].length*prob;
  }
  int next = sp[src*n+dst].next;
  if (next == dst) return sp[src*n+dst].length*prob;
  //cout << "enter " << src << endl;
  double total = 0;
  double fp = 1;
  int fpcount = 0;
  //int r;
  double sum = 1;
  set<int> randset[20];
  //cout << src <<  " " << dst << endl;
  if (net[src].size() == 0) {
    randset[0].insert(next);
    while (fp*falsepos*prob > threshold && fpcount < (int)neighbors[src].size() - 2) {
      fpcount ++;
      fp *= falsepos;
      //sum -= fp;
      /* randset[fpcount] = randset[fpcount -1 ];
      do {
        int pick = (int)(rand()/(double)RAND_MAX*neighbors[src].size()); 
        r = neighbors[src][pick];
      } while (net[src].find(r) != net[src].end());
      randset[fpcount].insert(r);*/
      int cont = 1;
      int choose[20];
      int i;
      for (i = 0; i < fpcount; i ++) {
        choose[i] = i;
      }
      while (cont) {
        set<int> newset;
        newset.insert(next);
        int enumer = 1;
        for (i = 0; i < fpcount; i ++) {
          newset.insert(neighbors[src][choose[i]]);
        }
        for (i = neighbors[src].size(); i > (int)neighbors[src].size() - fpcount; i --)
          enumer *= i;
        for (i = 0; i < fpcount; i ++) {
          enumer /= (i+1);
        }
        set<int> settmp = net[src];
        //net[src] = randset[fpcount];
        net[src] = newset;
        int size = net[src].size();
        if (net[src].find(prev) != net[src].end()) 
          size --;
        for (set<int>::iterator si = net[src].begin(); si != net[src].end(); si ++) {
          if (*si != prev) {
            total += sp[src*n+*si].length * prob * fp/ (double)size/(double) enumer + calc_latency(*si, dst, src, prob*fp/(double)size /(double) enumer);
            sum -= fp/(double)size/(double)enumer;
          }
        }
        net[src] = settmp;
        i = fpcount - 1;
        choose[i] ++;
        while (choose[i] > (int)neighbors[src].size() - (fpcount - i) && i >= 0) {
          choose[i] = 0;
          i --;
          choose[i] ++;
        }         
        if (i < 0) break;
        while (i < fpcount - 1) {
          i ++;
          choose[i] = choose[i-1] +1;
          if (choose[i] >= (int)neighbors[src].size()) {
            cont = 0; break;
          }
        }
      }
    }
    //cout << "next " << next << endl;
    set<int> settmp = net[src];
    net[src] = randset[0];
    total += sp[src*n+next].length * prob * sum  + calc_latency(next, dst, src, prob*sum);
    net[src] = settmp;
  } else {
    int size = net[src].size();
    if (net[src].find(prev) != net[src].end()) 
      size --;      
    for (set<int>::iterator si = net[src].begin(); si != net[src].end(); si ++)
      if (*si != prev)
        total += sp[src*n+*si].length * prob / (double)size + calc_latency(*si, dst, src, prob/(double)size);
  }
  //cout << src << " " << dst << " " << prob << " " << total << endl;
  //cout << "leave " << src << endl;
  return total;
}


int main(int argc, char ** argv) {  
  while (1) {
    char c = getopt(argc, argv, "d:f:");
    if (c == EOF) break;
    switch (c) {
    case 'd':
      if (strcmp(optarg, "isp") == 0) {
        datascheme= DATA_ISP;  
        //ntrace = 315;
      } else if (strcmp(optarg, "campus") == 0) {
        datascheme = DATA_CAMPUS;
      } else if (strcmp(optarg, "dc") == 0) {
        datascheme = DATA_DC;
      } 
      break;
    case 'f':
      fpexp = atoi(optarg);
      falsepos = exp10(-fpexp);
      cout << "false pos " << falsepos << endl;
      break;
    default:
      printf("?? getopt returned character code 0%o ??\n",c);
      break;
    }
  }

  char filename[255];
  if (datascheme == DATA_ISP)
    strcpy(filename, "conext/1239.bof");
  else if (datascheme == DATA_CAMPUS) 
    strcpy(filename, "conext/campus.bof");
  else if (datascheme == DATA_DC) 
    strcpy(filename, "conext/dc.bof");
  ifstream ifs(filename);
  //int link[MAXNODE][MAXNODE];
  Link link[MAXLINK];
  int node[MAXNODE];
  int i, m = 0;
  char ch;
  ifs >> n;
  double maxlen = 0;
  for (int j = 0; j < n; j ++) {
    ifs >> i >> ch;
    //sscanf(str, "%d %c", &i, &ch);
    if (ch == 'y') node[i] = 1;
    else node[i] = 0;
    //cout <<  i << ch << endl;
  }
  cout << "n " << n << endl;
  sp = new ShortestPath[n*n];
  ifs >> m; 
  for (i = 0; i < m; i ++) {
    ifs >> link[i].from >> link[i].to >> link[i].weight;
    if (link[i].weight > maxlen) maxlen = link[i].weight;
    //sscanf(str, "%d %d %lf", &link[i].from, &link[i].to, &link[i].weight);
    neighbors[link[i].from].push_back(link[i].to);
    neighbors[link[i].to].push_back(link[i].from);
  }
  cout << " m " << m << endl;
  calc_shortest_path(sp, n, m, link);

  cout << "max weight " << maxlen << endl;
  cout << "start calc stretch ... " << endl;

  //calculate stretch
  double avgstretch = 0, maxstretch = 0;
  int flowcount = 0;
  int src, dst;
  double minstretch = 9999;
  
#ifdef CALC_EXP
  if (datascheme == DATA_ISP)
    sprintf(filename, "conext/1239-%d.stretch", fpexp);
  else if (datascheme == DATA_CAMPUS) 
    sprintf(filename, "conext/campus-%d.stretch", fpexp);
  else if (datascheme == DATA_DC) 
    sprintf(filename, "conext/dc-%d.stretch", fpexp);
  ofstream outf(filename);
  double real_path  = 0;
  for (src = 0; src < n; src ++) {
    for (dst = src + 1; dst < n; dst ++) {
      if (node[src] && node[dst] && sp[src*n+dst].next > -1) {
        flowcount++;
        real_path += sp[src*n+dst].length;
        double stretch = (calc_latency(src, dst, -1, 1) - sp[src*n+dst].length) / sp[src*n+dst].length;
        avgstretch += stretch;
        outf << stretch << endl;
        if (stretch > maxstretch) {
          maxstretch = stretch;
        }
        if (stretch < minstretch) {
          minstretch = stretch;
        }
      }
    }
  }
  real_path/=(double)flowcount;
  cout << "avg path length " << real_path << endl;
#else
  if (datascheme == DATA_ISP)
    sprintf(filename, "conext/1239-%d-trace.stretch", fpexp);
  else if (datascheme == DATA_CAMPUS) 
    sprintf(filename, "conext/campus-%d-trace.stretch", fpexp);
  else if (datascheme == DATA_DC) 
    sprintf(filename, "conext/dc-%d-trace.stretch", fpexp);

  ofstream outf(filename);

  int zcount = 0;
  for (i = 0; i < 1e+6; i ++) {
    double r = rand();
    src = (int)(r*n/(double)RAND_MAX);
    r = rand();
    //int orig = src;
    dst = (int)(r*n/(double)RAND_MAX);
    if (src == dst || sp[src*n+dst].next < 0 || src >=n || dst >= n) {
      i --;
      continue;
    }
    flowcount++;
    int prev = -1;
    double reallength = sp[src*n+dst].length;
    //cout << "real len " << reallength << endl;
    double stretch = - reallength;
    //cout << "---> " << src << " " << dst << -stretch << endl;
    while (src != dst) {
      set<int> candidates;
      candidates.insert(sp[src*n+dst].next);
      for (vector<int>::iterator si=neighbors[src].begin(); si != neighbors[src].end(); si ++) {
        r = rand()/(double)RAND_MAX;
        if (r < falsepos && prev != *si && r != sp[src*n+dst].next) candidates.insert(*si);
      }
      prev = src;
      if (candidates.size() > 1) {
        int index = 0;
        do {
          r = rand()*candidates.size()/(double)RAND_MAX;
          index = (int) r;
        } while (index >= (int)candidates.size());
        set<int>::iterator si=candidates.begin();
        int i = 0;
        while (i < index && si != candidates.end()) {
          i ++;
          si ++;
        }
        src = *si;
      } else {
        src = sp[src*n+dst].next;
      }
      //cout << prev << " " << src << " " << sp[prev*n+src].length << " " << stretch << endl;
      stretch += sp[prev*n+src].length;
    }
    //if (stretch > 0) {
      //outf << "src " << orig << " src " << dst << " stretch " << stretch << " reallen " << reallength << " str/real " << stretch/reallength << endl;
      //outf << stretch << " " << stretch/reallength << endl;
    outf << stretch/reallength << endl;
      //cout << "heree eeee" << endl;
      //} else {
      //zcount ++;
      //}
   if (flowcount % (int)1e+6 == 0) {
      cout << "flow " << flowcount << endl;
    }
    avgstretch += stretch/reallength;
    if (stretch/reallength > maxstretch) {
      maxstretch = stretch/reallength;
    }
  }
  cout << " zero " << zcount << " total " << flowcount << endl;
#endif
  avgstretch/=(double)flowcount;
  cout << "max " << maxstretch << " avg " << avgstretch << " min " << minstretch << endl;
  exit(0);
}
