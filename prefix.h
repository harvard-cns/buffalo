#ifndef PREFIX_H
#define PREFIX_H

#include "util.h"

class prefix {
 public:
  uint32_t addr;
  uint8_t len;
  prefix() {
    addr = 0;
    len = 0;
  }
    prefix(uint32_t a, char l): addr(a), len(l) { }
    prefix(const prefix& p) {
      addr=p.addr;
      len=p.len;
    }
    void set(uint32_t a, char l) {
      addr=a; len=l;
    }
    prefix& operator=(const prefix& p) {
      addr=p.addr;
      len=p.len;
      return *this;
    }
    bool operator==(const prefix& p) const {
      if (addr ==p.addr  && len==p.len) {
        return true;
      }
      else {
        return false;
      }
    }
    bool operator!=(const prefix& p) const {
      if (addr ==p.addr  && len==p.len) {
        return false;
      }
      else {
        return true;
      }
    }
    bool operator<(const prefix& p) const {
      if (len>p.len) {
        return true;
      }
      else if (len==p.len) {
        return (addr <p.addr);
      }
      else {
        return false;
      }
    }

    bool contains(const prefix& p) const {
      if (len > p.len) return false;
      if (len == p.len && addr != p.addr) return false;
      uint32_t paddr = p.addr & (0xffffffff << (32-len));
      if (addr == paddr) return true; 
      return false;
    }

    friend ostream& operator<< (ostream &, const prefix &);
    friend istream& operator>> (istream &, prefix &);
};

ostream& operator<< (ostream &out, const prefix &p) {
  struct in_addr tmp; 
  tmp.s_addr=ntohl(p.addr);
  out << inet_ntoa(tmp) << "/" << (int)p.len;
  return out;
}

istream& operator >>(istream &in,prefix & p)
{
  int a, b, c, d;
  char str[IPLEN];
  in >> str;
  sscanf(str, "%d.%d.%d.%d/%u", &a, &b, &c, &d, (unsigned int *)&p.len);
  p.addr = (uint32_t)(a*(exp2(24)) + b*(exp2(16)) + c*(exp2(8)) + d);
  return in;
}


#endif
