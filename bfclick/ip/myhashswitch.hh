#ifndef CLICK_MYHASHSWITCH_HH
#define CLICK_MYHASHSWITCH_HH
#include <click/element.hh>
#include <click/etheraddress.hh>
#include <click/hashtable.hh>
#include "bloomfilter.hh"
CLICK_DECLS

class MyHashSwitch : public Element { public:

  MyHashSwitch();
  ~MyHashSwitch();

  const char *class_name() const		{ return "MyHashSwitch"; }
  const char *port_count() const		{ return "1/-"; }
  const char *processing() const		{ return PUSH; }
  const char *flow_code() const			{ return "#/[^#]"; }

    int configure(Vector<String> &, ErrorHandler *);
    void add_handlers();

  void push(int port, Packet* p);

  /*struct AddrInfo {
    int port;
    Timestamp stamp;
    inline AddrInfo(int p, const Timestamp &t);
    };*/
  
  private:

  //typedef HashTable<EtherAddress, AddrInfo> Table;
  //  Table _table;
  HashTable<EtherAddress, int> _table;

  //bloom_filter bf;

    uint32_t _timeout;

    void broadcast(int source, Packet*);

    static String reader(Element *, void *);
    static int writer(const String &, Element *, void *, ErrorHandler *);
    friend class ListenMyHashSwitch;

};

CLICK_ENDDECLS
#endif
