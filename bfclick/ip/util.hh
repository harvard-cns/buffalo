#ifndef UTIL_H
#define UTIL_H

//#define BF_RANDOM click_random
//#define BF_SRANDOM click_srandom

#define BF_RANDOM 
#define BF_SRANDOM 


/*
extern uint32_t bf_random_seed;
extern uint32_t bf_curr_rand;

inline uint32_t bf_random() {
#if CLICK_BSDMODULE
  return random();
#elif CLICK_LINUXMODULE
  bf_random_seed = bf_random_seed * 69069L + 5;
  return (bf_random_seed ^ jiffies) & CLICK_RAND_MAX; // XXX jiffies??                                                                                  
#elif HAVE_RANDOM && CLICK_RAND_MAX == RAND_MAX
  //uint32_t tmp = random();
  srandom(bf_curr_rand);
  bf_curr_rand = random();
  srandom(time(0));
  return bf_curr_rand;
#else
  //uint32_t tmp = rand();
  srand(bf_curr_rand);
  bf_curr_rand = rand();
  srand(time(0));
  return bf_curr_rand;
#endif
}

inline void bf_srandom(uint32_t seed) {
#if CLICK_BSDMODULE
  srandom(seed);
#elif !CLICK_LINUXMODULE && HAVE_RANDOM && CLICK_RAND_MAX == RAND_MAX
  srandom(seed);
  bf_random_seed = seed;
  bf_curr_rand = seed;
#elif !CLICK_LINUXMODULE && CLICK_RAND_MAX == RAND_MAX
  srand(seed);
#else
  bf_random_seed = seed;
#endif
}*/

#endif
