#include <fstream>
#include <iterator>
#include <algorithm>
#include <map>
#include <set>

using namespace std;

#include "util.h"
#include "BloomFilter.h"
#include "prefix.h"
#include "etheraddress.h"

#define OUTPUT_FALSEPOS_DETAIL

const int HASH_BIT = 1;

int SPACERATE=32;

int MEM_SIZE = 1024*1024*8; //1MByte
int lambda = 0;
//#define OUTPUT_DETAILS 1

vector<prefix> pfxlist;
map<prefix, uint32_t> pmap; 
set<uint32_t> nhlist;
map<uint32_t, unsigned int> nhindex;
map<uint32_t, unsigned int> nhcount;

vector<EtherAddress> maclist;
map<EtherAddress, uint32_t> macmap;

enum dataschemetype {DATA_ROUTEVIEW, DATA_FUNET, DATA_INTERNET2, DATA_MAC};

schemetype runscheme = SCHEME_LPM;
dataschemetype datascheme = DATA_FUNET;

#ifdef OUTPUT_FALSEPOS_DETAIL
FILE * fdetail;
FILE * fcorrect;       
#endif 

string schemetype_tostring(schemetype val) {
  if (val==SCHEME_LPM) return "SCHEME_LPM";
  else if (val==SCHEME_FLAT) return "SCHEME_FLAT";
  else {
    abort();
    return "ERROR";
  }
}

bloom_filter * filter = new bloom_filter[MAX_BLOOM_FILTER];

#define PFXLEN_COUNT 33

//SCHEME_OUT_PFXLEN
map<unsigned char, unsigned int> pfxlencount;
bloom_filter * pfxlenfilter = new bloom_filter[PFXLEN_COUNT];

//SCHEME_PFXLEN_EACH_OUT
map<uint32_t, unsigned int> nh_pfxlen_count[PFXLEN_COUNT];
bloom_filter * nh_pfxlen_filter = new bloom_filter[PFXLEN_COUNT*MAX_BLOOM_FILTER];

void create_bloom_filter_pfxlen_each_out() {
  long unsigned int table_size = 0;

  double total_n = 0, total_nlnn = 0;
  for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
    for (int j = 1; j <=32; j ++) {
      if (nh_pfxlen_count[j][*si] > 0) {
        int n = nh_pfxlen_count[j][*si];
        total_n += n;
        total_nlnn += n * log(n);
        if (n > 1) {
          cout << "nexthop " << *si << " pfxlen " << j << " n(t) " << n << " nln(n) " << n*log(n) << endl;
        }
      }
    }
  }
  
  double lambda = (MEM_SIZE*log(2)*log(2)+total_nlnn)/total_n;

  cout << "total_n " << total_n << " total_nlnn " << total_nlnn << " lambda " << lambda << endl;
  
  for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
    for (int j = 1; j <=32; j ++) {
      int n = nh_pfxlen_count[j][*si];
      int size = 0;
      if (n > 0) 
        //size = (int)((lambda-log(n))*n/(log(2)*log(2)));
        size = (int)((double)MEM_SIZE*n/total_n);
      else size = 1;
      //nh_pfxlen_filter[nhindex[*si]*PFXLEN_COUNT+j].bloom_filter_init(nh_pfxlen_count[j][*si]*SPACERATE, nh_pfxlen_count[j][*si], 10);
      nh_pfxlen_filter[nhindex[*si]*PFXLEN_COUNT+j].bloom_filter_init(size, n, 10);
      //table_size += nh_pfxlen_count[j][*si];
      table_size += size;
      if (size > 1) {
        cout << "nexthop " << *si << " pfxlen " << j << " m(t) " << size << endl;
      }
    }
  }
  table_size /=(nhlist.size()*32);
  cout << " avg table_size " << table_size << endl;
  for (unsigned int i = 0; i < pfxlist.size(); i ++) {
    nh_pfxlen_filter[nhindex[pmap[pfxlist[i]]]*PFXLEN_COUNT+pfxlist[i].len].insert(pfxlist[i]);
  }
  /*  for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
    for (int j = 1; j <=32; j ++) {
      nh_pfxlen_filter[nhindex[*si]*PFXLEN_COUNT+j].check();
    }
    }*/
}


void create_mac_table() {
  long unsigned int table_size = 0;

  ifstream sizeinfile("size.txt");

  for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
      int n = nhcount[*si];
      int size = 0;
      sizeinfile >> size;
      nh_pfxlen_filter[nhindex[*si]].bloom_filter_init(size, n, 10);
      table_size += size;
      if (size > 1) {
        cout << "nexthop " << *si << " m(t) " << size << endl;
      }
  }
  table_size /=(nhlist.size()*32);
  cout << " avg table_size " << table_size << endl;
  for (unsigned int i = 0; i < maclist.size(); i ++) {
    nh_pfxlen_filter[nhindex[macmap[maclist[i]]]].insert(maclist[i]);
  }

  sizeinfile.close();
}

int xseed = (unsigned)time(NULL);

uint32_t randomip() {
  srand( xseed );
  xseed = rand();

  return (uint32_t)(xseed/(double)RAND_MAX * exp2(32));
}

ifstream ipaddrfp("packet.txt");
unsigned long long packet_count = 0;

uint32_t fetchip() {
  uint32_t addr;  
  char str[IPLEN];
  //int i;
  //int min = 0;
  
  if (datascheme == DATA_FUNET) {
    if (ipaddrfp >> addr) {
      /*uint32_t mask = 0xffffffff;
        for (i = 0; i < 32; i ++) {
        if ((addr & (mask << i)) != addr) {
        cout << i << endl;
        break;
        }
        }*/
      packet_count ++;
      
      return addr;
    } 
  }

  if (datascheme == DATA_INTERNET2) {
    if (ipaddrfp >> str) {
      addr = str2ip(str);
      srand( xseed );
      xseed = rand();
      
      packet_count ++;

      return (uint32_t)(xseed/(double)RAND_MAX * exp2(11) + addr);     
    }
  }

  cout << " file end " << endl;
  return 0;
}

int fetchmac(EtherAddress & addr) {
  if (ipaddrfp >> addr) {
    packet_count ++;
    return 1;
  } else return 0;
}

uint32_t linear_lookup(uint32_t addr, int len) {
  uint32_t mask = 0xffffffff;
  for (int i = len; i > 0; i --) {
    prefix p;
    p.len = i;
    p.addr = addr & mask;
    if (pmap.find(p) != pmap.end()) 
      return pmap[p];
    mask = (mask << 1) & 0xffffffff;
  }
  return 0;
}

int test_prefix_nexthop() {
  set<prefix> spfx;
  unsigned int i;
  for (i = 0; i < pfxlist.size(); i ++) {
    prefix pi;
    pi.addr = pfxlist[i].addr;
    pi.len = pfxlist[i].len;
    spfx.insert(pi);
  }
  sort(pfxlist.begin(), pfxlist.end());
  for (vector<prefix>::iterator vi = pfxlist.begin(); vi != pfxlist.end(); vi ++) {
    //int count = 0;
    set<uint32_t> nhset;
    prefix pvi;
    pvi.addr = vi->addr;
    pvi.len = vi->len;
    if (spfx.find(pvi) != spfx.end()) {
      for (i = pvi.len; i > 0; i --) {
        prefix p;
        p.len = i;
        p.addr = (pvi.addr & (0xffffffff << (32-i)));
        if (pmap.find(p) != pmap.end()) {
          spfx.erase(p);
          nhset.insert(pmap[p]);
          //if (pmap[p] != pmap[pvi]) count ++;
          //count ++;
        }
      }
      cout << nhset.size() << endl;
      /*if (!count) {
        cout << "hello" << endl;
        cout << pvi << endl;
        }*/
    }
  }
  return 0;
}

double perform_pfxlen_each_out() {
  cout << " enter perform .. " << endl;
  int falsepos = 0, falseneg = 0;
   
  unsigned long long test_samples = 0;
  if (datascheme == DATA_ROUTEVIEW) 
    test_samples = pfxlist.size();
  else if (datascheme == DATA_FUNET || datascheme == DATA_INTERNET2) test_samples = 999999;
   
   int maxerror = 0, avgerror = 0;

   for (unsigned int i = 0; i < test_samples; i ++) {
     //     cout << i << endl;
     prefix p; 
     //uint32_t addr = randomip();
     uint32_t addr;
     if (datascheme == DATA_ROUTEVIEW) addr = randomip();
     else if (datascheme == DATA_FUNET || datascheme == DATA_INTERNET2) {
       addr = fetchip();
       if (addr ==0) break;
       if (i > 10) i =5;
     }
     uint32_t mask = 0xffffffff;
     int error = 0;
     uint32_t nh = 0;
     int hitj = 0;
     if (HASH_BIT) {       
       for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
         uint32_t mask = 0xffffffff;
         int j;
         for (j = 32; j > 0; j --) {
           p.len = j;
           p.addr = addr & mask;
           if (nh_pfxlen_filter[nhindex[*si]*PFXLEN_COUNT+j].contains(p)) {
             if (nh == 0 || j > hitj) {
               hitj = j;
               nh = *si;
             }
             error ++;
             break;
           }
           mask <<= 1;
        }
         //if (error) break;
         /*if (j > 0) {
           cout << "(" << nhindex[*si] << " " << p<< ") ";
           }*/
       }
       if (error == 0) {
         /*for (int j = 0; j < 32; j ++) {
           p.len = j;
           mask = 0xffffffff;
           mask <<= (32 - j);
           p.addr = addr & mask;
           cout << "(" << p << ")";
         }
         cout << endl;*/
         i --; 
         packet_count --;
         continue;
       }

       uint32_t linearnh = linear_lookup(addr, 32);
       if (linearnh == 0) {
         i --;
         packet_count --;
         continue;
       }

       if (nh != linearnh) {
         if (error <= 1) {
           prefix p;
           p.addr = addr;
           p.len = 32;
           prefix pnh;
           pnh.addr = nh;
           pnh.len = 32;
           cout << "lookup error " << p << " bf nh " << pnh << " linear nh " << linearnh<< endl;
      
           /* cout << " bf lookup " << endl;
           nh = 0;
           hitj = 0;
           for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
             uint32_t mask = 0xffffffff;
             int j;
             for (j = 32; j > 0; j --) {
               p.len = j;
               p.addr = addr & mask;
               if (nh_pfxlen_filter[nhindex[*si]*PFXLEN_COUNT+j].contains(p)) {
                 if (nh == 0 || j > hitj) {
                   hitj = j;
                   nh = *si;
                 }
                 pnh.addr = *si;
                 pnh.len = 32;
                 cout << "(nh " << pnh << " len " << j << ")";
                 break;
               }
               mask <<= 1;
             }
           }
           cout << endl;
           cout << " linear lookup " << endl;
           uint32_t mask = 0xffffffff;
           for (int j = 32; j > 0; j --) {
             prefix p;
             p.len = j;
             p.addr = addr & mask;
             if (pmap.find(p) != pmap.end()) 
               cout << "(nh " << pmap[p] << " len " << j << endl;
             mask <<= 1;
             }*/
         }
         falseneg ++;
       }
       //cout << endl;
       if (error > 1) {
         falsepos ++;
      }

#ifdef OUTPUT_FALSEPOS_DETAIL
       if (error > 1) {
         fprintf(fdetail, "%u\n", addr);
       } else {
         fprintf(fcorrect, "%u\n", addr);
       }
#endif 

       avgerror += error;
       if (error > maxerror) maxerror = error;

       //cout << falseneg/(double)(i+1) << " " << falsepos/(double)(i+1) << endl;

       //cout << error << endl;     
     } else { //HASH_BIT
       //HANDLE DK bit...
       int * nhmaxlen = new int[nhlist.size()];
       bzero(nhmaxlen, sizeof(int)*nhlist.size());
       int find = 0;
       for (int j = 32; j > 0; j --) {
         p.len = j;
         p.addr = addr & mask;
         for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
           if (nhmaxlen[nhindex[*si]] == 0) {
             int check = nh_pfxlen_filter[nhindex[*si]*PFXLEN_COUNT+j].contains(p);
             if (check == 1) {
               nhmaxlen[nhindex[*si]] = j;
               //cout << "(" << nhindex[*si] << " " << j << " " << check << endl;
               error ++;
               nh = *si;
               hitj = j;
               if (find == 0) {
                 find = 1;
               }
               break;
             } 
             if (check == 2) {
               //cout << "(" << nhindex[*si] << " " << j << " " << check << endl;
               error ++;
               if (find == 0) find = 2;
               nhmaxlen[nhindex[*si]] = j;
             }
           }
         }
         //if (hitj == j) break;
         mask <<= 1;
       }
       if (error > 1) {
         for (unsigned int j = 0; j < nhlist.size(); j ++) {
           if (nhmaxlen[j] > 0) {
             p.len = nhmaxlen[j];
             mask = 0xffffffff;
             mask <<= (32 - p.len);
             p.addr = addr & mask;
             cout << "(" << j << " " << p << " " << nh_pfxlen_filter[j*PFXLEN_COUNT+nhmaxlen[j]].contains(p) << ")";
           }
         }
         cout << endl;
         if (find == 0 || find == 2) falsepos ++;
       }
       delete(nhmaxlen);
     } 
   }

   if (datascheme == DATA_FUNET || datascheme == DATA_INTERNET2)
     test_samples = packet_count;
   
   cout << "test_samples " << test_samples << " false negative " << falseneg /  (double) test_samples << " concurrent netxhops max " << maxerror << " avg " << avgerror/(double)test_samples << endl;

   return falsepos/(double)test_samples;
}



double perform_mac_lookup() {
  cout << " enter perform .. " << endl;
  int falsepos = 0;
   
  unsigned long long test_samples = 0;

  int maxerror = 0, avgerror = 0;

  EtherAddress p;
  while (fetchmac(p)) {
    packet_count ++;
    int error = 0;
    int hitj = 0;
    for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
      int check = nh_pfxlen_filter[nhindex[*si]].contains(p);
      if (check == 1) {
        error ++;
        hitj = *si;
      } 
    }
    if (error > 1) falsepos ++;
    if (error > maxerror) maxerror = error;
    avgerror += error;
  }
  test_samples = packet_count;
   
   cout << "test_samples " << test_samples  << " concurrent netxhops max " << maxerror << " avg " << avgerror/(double)test_samples << endl;

   return falsepos/(double)test_samples;
}

void read_routeviewfib(char * filename) {
  ifstream ifs(filename);
   prefix p;
   uint32_t nexthop;
   char str[IPLEN];
   while (ifs >> p >> str) {
     nexthop = str2ip(str);
     if (runscheme == SCHEME_FLAT && p.len != 24) 
       continue;
     pmap[p] = nexthop;
     pfxlist.push_back(p);
     nhlist.insert(nexthop);
     nhcount[nexthop] ++;
     pfxlencount[p.len] ++;

     nh_pfxlen_count[p.len][nexthop] ++;
   }

}

void read_funetfib(char * filename) {
   ifstream ifs(filename);
   prefix p;
   uint32_t tmp, nexthop;
   uint32_t addr, len;
   while (ifs >> addr >> len >> tmp) {
     nexthop = tmp + 100;
     p.addr = addr;
     p.len = len;
     if (runscheme == SCHEME_FLAT && p.len != 24) 
       continue;
     pmap[p] = nexthop;
     pfxlist.push_back(p);
     nhlist.insert(nexthop);
     nhcount[nexthop] ++;
     pfxlencount[p.len] ++;

     nh_pfxlen_count[p.len][nexthop] ++;
   }
}

void read_macfib(char * filename) {
   ifstream ifs(filename);
   EtherAddress p;
   uint32_t tmp, nexthop;
   while (ifs >> p >> tmp) {
     nexthop = tmp + 100;
     maclist.push_back(p);
     macmap[p] = nexthop;
     nhlist.insert(nexthop);
     nhcount[nexthop] ++;
   }
}

int main(int argc, char* argv[]) {
#ifdef OUTPUT_FALSEPOS_DETAIL
  fdetail = fopen("detail.txt", "w");
  fcorrect = fopen("correct.txt", "w");
#endif

  //read parameters
  char filename[255];
  strcpy(filename, "fib.txt");
  while (1) {
    char c = getopt(argc, argv, "r:m:f:d:");
    if (c == EOF) break;
    switch (c) {
    case 'r':
      SPACERATE = atoi(optarg);
      MEM_SIZE =(int)(MEM_SIZE*atof(optarg));
      break;
    case 'm':
      if (strcmp(optarg, "flat") == 0) {
        runscheme = SCHEME_FLAT;
      } else if (strcmp(optarg, "lpm") == 0) {
        runscheme = SCHEME_LPM;
      }
      break;
    case 'f':
      strcpy(filename, optarg);
      break;
    case 'd':
      if (strcmp(optarg, "routeview") == 0) {
        datascheme= DATA_ROUTEVIEW;  
      } else if (strcmp(optarg, "funet") == 0) {
        datascheme = DATA_FUNET;
      } else if (strcmp(optarg, "internet2") == 0) {
        datascheme = DATA_INTERNET2;
      } else if (strcmp(optarg, "mac") == 0) {
        datascheme = DATA_MAC;
        runscheme = SCHEME_FLAT;
      }
      break;
    default:
      printf("?? getopt returned character code 0%o ??\n",c);
      break;
    }
  }

  if (datascheme == DATA_ROUTEVIEW) {
    read_routeviewfib(filename);
  } else if (datascheme == DATA_FUNET) {
    read_funetfib(filename);
  } else if (datascheme == DATA_INTERNET2) {
    read_routeviewfib(filename);
  } else if (datascheme == DATA_MAC) {
    read_macfib(filename);
  }

   /*cout << "pfx size "<< pfxlist.size() << endl;
   cout << "nh size " << nhlist.size() << endl;
   
   for (int j = 0; j <=32; j ++) {
     cout << "pfxlen " << j << " " << pfxlencount[j] << " " << pfxlencount[j]/((double)(pfxlist.size())) << endl;
     }

   exit(0);
   */
   //test_prefix_nexthop();
   //exit(0);
   
   int i = 0;
   for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
     nhindex[*si] = i;
     //cout << " nexthop " << *si << " count " << nhcount[*si] << endl;
     i ++;
   }

   cout << "create bloom filter" << endl;
   /*   if (runscheme == SCHEME_SIMPLEOUT) {
     create_bloom_filter_simpleout();
     cout << "test false positive" << endl;
     double falsepos = test_each_entry();
     double falsepos2 = test_simpleout();
     cout << "false positive rate -- fib entry " << falsepos << endl;
     cout << "false positive rate -- random prefix " << falsepos2 << endl;
   }

   if (runscheme == SCHEME_OUT_PFXLEN) {
     create_bloom_filter_simpleout();
     create_bloom_filter_pfxlen();
     cout << "test false positive" << endl;
     double falsepos = test_each_entry();
     double falsepos2 = test_out_pfxlen();
     cout << "false positive rate -- fib entry " << falsepos << endl;
     cout << "false positive rate -- random prefix " << falsepos2 << endl;   
   }
   */
   
   double falsepos2 = 0;

   if (datascheme == DATA_MAC) {
     create_mac_table();     
   } else {
     create_bloom_filter_pfxlen_each_out();
   }

   cout << "test false positive" << endl;

   if (datascheme == DATA_MAC) {
     falsepos2 = perform_mac_lookup();
   } else {
     //double falsepos = test_each_entry_pfxlen_each_out();
     falsepos2 = perform_pfxlen_each_out();
   }

   //printf("false positive rate -- fib entry %lf\n", falsepos);
   printf("false positive rate -- random prefix %.10lf %e\n ", falsepos2, falsepos2);   
   
#ifdef OUTPUT_FALSEPOS_DETAIL
   fclose(fdetail);
   fclose(fcorrect);
#endif

}
