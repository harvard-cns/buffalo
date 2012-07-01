#ifndef ETHERADDRESS_H
#define ETHERADDRESS_H

#include "util.h"
class EtherAddress {
 public:
  uint16_t addr[3];
  EtherAddress() {
    addr[0] = addr[1] = addr[2] = 0;
  }
  //EtherAddress(uint32_t a, char l): addr(a), len(l) { }
  EtherAddress(const EtherAddress& p) {
    addr[0]=p.addr[0];
    addr[1]=p.addr[1];
    addr[2]=p.addr[2];
  }
  EtherAddress(uint32_t a) {
    addr[0] = 0x3434;
    addr[1] = a >> 16;
    addr[2] = a << 16 >> 16;
  }
  void set(uint16_t *p) {
    addr[0]=p[0];
    addr[1]=p[1];
    addr[2]=p[2];
  }
  EtherAddress& operator=(const EtherAddress& p) {
    addr[0]=p.addr[0];
    addr[1]=p.addr[1];
    addr[2]=p.addr[2];
    return *this;
  }
    bool operator==(const EtherAddress& p) const {
      if (addr[0] ==p.addr[0]  && addr[1] == p.addr[1] && addr[2] == p.addr[2]) {
        return true;
      }
      else {
        return false;
      }
    }
    bool operator!=(const EtherAddress& p) const {
      if (addr[0] ==p.addr[0]  && addr[1] == p.addr[1] && addr[2] == p.addr[2]) {
        return false;
      }
      else {
        return true;
      }
    }
    bool operator<(const EtherAddress& p) const {
      if (addr[0] < p.addr[0]) {
        return true;
      }
      else if (addr[0] == p.addr[0]) {
        return (addr[1] < p.addr[1] || (addr[1] == p.addr[1] && addr[2] < p.addr[2]));
      }
      else {
        return false;
      }
    }

    friend ostream& operator<< (ostream &, const EtherAddress &);
    friend istream& operator>> (istream &, EtherAddress &);
};

ostream& operator<< (ostream &out, const EtherAddress &p) {
  
  out << hex << (p.addr[0] >> 8) << ":" << (p.addr[0] & 0x00ff)  << ":" 
      << (p.addr[1] >> 8) << ":" << (p.addr[1] & 0x00ff) << ":" 
      << (p.addr[2] >> 8) << ":" << (p.addr[2] & 0x00ff) ;
  return out;
}

istream& operator >>(istream &in,EtherAddress & p)
{
  int a, b, c, d, e, f;
  char str[IPLEN];
  in >> str;
  sscanf(str, "%x:%x:%x:%x:%x:%x", &a, &b, &c, &d, &e, &f);
  p.addr[0] = (a << 8) + b;
  p.addr[1] = (c << 8) + d;
  p.addr[2] = (e << 8) + f;
  return in;
}

class Flow {
 public:
  EtherAddress from, to;
  Flow() {
  }
  //Flow(uint32_t a, char l): addr(a), len(l) { }
  Flow(const Flow& p) {
    from = p.from;
    to = p.to;
  }
  Flow& operator=(const Flow& p) {
    from = p.from;
    to = p.to;
    return *this;
  }
    bool operator==(const Flow& p) const {
      if (from == p.from && to == p.to) {
        return true;
      }
      else {
        return false;
      }
    }
    bool operator!=(const Flow& p) const {
      if (from == p.from && to == p.to) {
        return false;
      }
      else {
        return true;
      }
    }
    bool operator<(const Flow& p) const {
      if (from < p.from) {
        return true;
      }
      else if (from == p.from) {
        return (to < p.to);
      }
      else {
        return false;
      }
    }
};

#endif
