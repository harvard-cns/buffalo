//test random packets
double test_each_entry_pfxlen_each_out() {
   //test false positive 
   int falsepos = 0;
   for (unsigned int i = 0; i < pfxlist.size(); i ++) {
     char find = 0; 
     int match_count = 0;
     for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
       if (nh_pfxlen_filter[nhindex[*si]*PFXLEN_COUNT + pfxlist[i].len].contains(pfxlist[i])) {
         match_count ++;
         if (match_count > 1) break;
         //cout << pfxlist[i] << " " << pmap[pfxlist[i]] << " " << *si << endl;
         if (pmap[pfxlist[i]] == *si) {
           find = 1;
         } else {
           //perror("cannot find mapping");
         }
       }
     }
     if (match_count > 1) {
       /*cout << " prefix " << pfxlist[i] << " real nexthop " << pmap[pfxlist[i]]  << endl;
       for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
         if (nh_pfxlen_filter[nhindex[*si]*PFXLEN_COUNT+pfxlist[i].len].contains(pfxlist[i])) {
           cout << " hit " << *si << endl;
           cout << " table size " << nh_pfxlen_count[pfxlist[i].len][*si] << endl;
         }   
       }     */
       falsepos ++;
     }
   }
   return falsepos/(double)pfxlist.size();
}

double test_simpleout() {
   //test false positive
   int falsepos = 0;
   
   unsigned int test_samples = pfxlist.size();

   for (unsigned int i = 0; i < test_samples; i ++) {
     prefix p; 
     uint32_t addr = randomip();
     uint32_t mask = 0xffffffff;
     char error = 0;
     //cout << " test " << i << " " << addr << endl;
     for (int j = 32; j > 0; j --) {
       //cout << " pfxlen " << j << endl;
       p.len = j;
       p.addr = addr & mask;
#ifdef OUTPUT_DETAILS
       cout << " addr " << addr << " prefix " << p << endl;
#endif
       if (pmap.find(p) == pmap.end()) {
         for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
           if (filter[nhindex[*si]].contains(p)) {
             error = 1;
#ifdef OUTPUT_DETAILS
             cout << "false positive " << p << endl;
#endif
             break;       
           }
         }
         if (error) {
           falsepos ++;
           break;
         }
       } else {
         uint32_t realnexthop = pmap[p];
         char find = 0; 
         int match_count = 0;
         for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
           if (filter[nhindex[*si]].contains(p)) {
             match_count ++;
             if (match_count > 1) break;
             if (realnexthop == *si) {
               find = 1;
             } else {
               //perror("cannot find mapping");
             }
           }
         }
         /*if (!find) {
           cout << " prefix " << p << " real nexthop " << realnexthop << endl;
           for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
             if (filter[nhindex[*si]].contains(p)) {
               cout << " hit " << *si << endl;
             }   
           }     
           perror("cannot find mapping");
           }*/
         if (match_count > 1) {
           falsepos ++;
#ifdef OUTPUT_DETAILS
           cout << "false positive " << p << endl;
#endif
         }   
         break;
       }
       mask <<= 1;
     }
   }
     
   return falsepos/(double)test_samples;
}

double test_out_pfxlen() {
   int falsepos = 0;
   
   unsigned int test_samples = pfxlist.size();

   for (unsigned int i = 0; i < test_samples; i ++) {
     prefix p; 
     uint32_t addr = randomip();
     uint32_t mask = 0xffffffff;
     char error = 0;
     for (int j = 32; j > 0; j --) {
       p.len = j;
       p.addr = addr & mask;
       if (pmap.find(p) == pmap.end()) {
         if (pfxlenfilter[j].contains(p)) {
           for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
             if (filter[nhindex[*si]].contains(p)) {
               error = 1;
               break;       
             }
           }
           if (error) {
             falsepos ++;
             break;
           }
         } 
       } else {
         if (!pfxlenfilter[j].contains(p)) {
           perror("pfxlen");
           exit(0);
         }
         uint32_t realnexthop = pmap[p];
         char find = 0; 
         int match_count = 0;
         for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
           if (filter[nhindex[*si]].contains(p)) {
             match_count ++;
             if (match_count > 1) break;
             if (realnexthop == *si) {
               find = 1;
             }
           }
         }
         if (match_count > 1) {
           falsepos ++;
         }   
         break;
       }
       mask <<= 1;
     }
   }
     
   return falsepos/(double)test_samples;
}

double test_pfxlen_each_out() {
   int falsepos = 0;
   
   unsigned int test_samples = pfxlist.size();

   for (unsigned int i = 0; i < test_samples; i ++) {
     prefix p; 
     uint32_t addr = randomip();
     uint32_t mask = 0xffffffff;
     char error = 0;
     for (int j = 32; j > 0; j --) {
       p.len = j;
       p.addr = addr & mask;
       if (pmap.find(p) == pmap.end()) {
         for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
           if (nh_pfxlen_filter[nhindex[*si]*PFXLEN_COUNT+j].contains(p)) {
             error = 1;
             break;       
           }
         }
         if (error) {
           falsepos ++;
           break;
         }
       } else {
         uint32_t realnexthop = pmap[p];
         char find = 0; 
         int match_count = 0;
         for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
           if (nh_pfxlen_filter[nhindex[*si]*PFXLEN_COUNT+j].contains(p)) {
             match_count ++;
             if (match_count > 1) break;
             if (realnexthop == *si) {
               find = 1;
             }
           }
         }
         if (match_count > 1) {
           falsepos ++;
         }   
         break;
       }
       mask <<= 1;
     }
   }
     
   return falsepos/(double)test_samples;
}

///test entry
double test_each_entry() {
   //test false positive 
   int falsepos = 0;
   for (unsigned int i = 0; i < pfxlist.size(); i ++) {
     char find = 0; 
     int match_count = 0;
     for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
       if (filter[nhindex[*si]].contains(pfxlist[i])) {
         match_count ++;
         if (match_count > 1) break;
         //cout << pfxlist[i] << " " << pmap[pfxlist[i]] << " " << *si << endl;
         if (pmap[pfxlist[i]] == *si) {
           find = 1;
         } else {
           //perror("cannot find mapping");
         }
       }
     }
     /*if (!find) {
       cout << " prefix " << pfxlist[i] << " real nexthop " << pmap[pfxlist[i]] << endl;
       for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
         if (filter[nhindex[*si]].contains(pfxlist[i])) {
           cout << " hit " << *si << endl;
         }   
       }     
       perror("test_each_entry: cannot find mapping");
       }*/
     if (match_count > 1) falsepos ++;
   }
   return falsepos/(double)pfxlist.size();
}



//////////////////BF creation methods//////////////////

//create outgoing link bloom filters   
void create_bloom_filter_simpleout() {
  long unsigned int table_size = 0;
   for (set<uint32_t>::iterator si = nhlist.begin(); si != nhlist.end(); si ++) {
     //cout << "nhindex " << nhindex[*si] << endl;
     filter[nhindex[*si]].bloom_filter_init(nhcount[*si]*SPACERATE, nhcount[*si], 10);
     table_size += nhcount[*si];
     for (unsigned int j  = 0; j < pfxlist.size(); j ++) {
       if (pmap[pfxlist[j]] == *si) {
         //cout << pfxlist[j] << " " << pmap[pfxlist[j]] << " " << *si << endl;
         filter[nhindex[*si]].insert(pfxlist[j]);
       }
     }
   }
   table_size /=nhlist.size();
   cout << " avg table_size " << table_size << endl;
}

//create outgoing link bloom filters   
void create_bloom_filter_pfxlen() {
  long unsigned int table_size = 0;
  for (int i = 1; i <= 32; i ++) {
     pfxlenfilter[i].bloom_filter_init(pfxlencount[i]*SPACERATE, pfxlencount[i], 10);
     table_size += pfxlencount[i];
     for (unsigned int j  = 0; j < pfxlist.size(); j ++) {
       if (pfxlist[j].len == i) {
         //cout << pfxlist[j] << " " << pmap[pfxlist[j]] << " " << *si << endl;
         pfxlenfilter[i].insert(pfxlist[j]);
       }
     }
   }
   table_size /=32;
   cout << " avg table_size " << table_size << endl;
}

//universal hash functions
class HashFunction {
 public:
  int b; // 32 bit prefix + 8 bit pfxlen
  int seed[40];
  HashFunction(int range) {
    b = 40;
    int i;
    //cout << "seeds: \n";
    for (i = 0; i < b; i ++) {
      seed[i] = (int)(rand()/(double)RAND_MAX * range);
      //cout << seed[i];
    }
    //cout << endl;
  }
  unsigned int hash(prefix pfx) {
    unsigned int bit = 1;
    int result = 0;
    int i;
    uint32_t a = pfx.addr;
    for (i = 0; i < 32; i ++) {
      result ^= seed[i] * (bit & a);
      a >>= 1;
    }
    //if (runscheme != SCHEME_PFXLEN_EACH_OUT) {
    a = pfx.len;
    for (i = 0; i < b - 32; i ++) {
      result ^= seed[i+32] * (bit & a);
      a >>= 1;
    }
    //}
    return result;
  }
};
