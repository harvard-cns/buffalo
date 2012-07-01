/*
 * bfswitch.{cc,hh} -- learning, forwarding Ethernet bridge
 * John Jannotti
 *
 * Copyright (c) 1999-2000 Massachusetts Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, subject to the conditions
 * listed in the Click LICENSE file. These conditions include: you must
 * preserve this copyright notice, and you cannot mention the copyright
 * holders in advertising related to the Software without their permission.
 * The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
 * notice is a summary of the Click LICENSE file; the license in that file is
 * legally binding.
 */

#include <click/config.h>
#include "myhashswitch.hh"
#include <clicknet/ether.h>
#include <click/etheraddress.hh>
#include <click/glue.hh>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include <click/error.hh>

CLICK_DECLS

MyHashSwitch::MyHashSwitch()
{
  //bf.initialize();
}

MyHashSwitch::~MyHashSwitch()
{
}

int my_cp_ether_route(String s, EtherAddress * eaddr, int * eport, Element *context)
{
  unsigned char tmp[3];
  int port;
  if (!cp_ethernet_address(cp_pop_spacevec(s), tmp, context))
    return false;

  EtherAddress addr(tmp);

  String word = cp_pop_spacevec(s);
  if (word == "-")
    // null gateway; do nothing 
    ;
  //else 
    //if (cp_ip_address(word, &r.gw, context))
    // do nothing 
    //;
  else
    goto two_words;
    word = cp_pop_spacevec(s);
 two_words:
  if (cp_integer(word, &port) || (!word))
    if (!cp_pop_spacevec(s)) { // nothing left
      *eaddr = addr;
      *eport = port;
      return true;
    }
  
  return false;
}

int
MyHashSwitch::configure(Vector<String> &conf, ErrorHandler *errh)
{
  /*return cp_va_kparse(conf, this, errh,
			"TIMEOUT", 0, cpSeconds, &_timeout,
			cpEnd);
  */
  errh = NULL;
  EtherAddress addr;
  int port;

  for (int i = 0; i < conf.size(); i ++) {
    if (!my_cp_ether_route(conf[i], &addr, &port, this))
      return -1;
    
    //click_chatter("%s %d", addr.unparse_colon().c_str(), port);
    
    _table[addr] = port;
    //bf.etherinsert(addr, port);
  }

  return 0;
}

void
MyHashSwitch::broadcast(int source, Packet *p)
{
  int n = noutputs();
  assert((unsigned) source < (unsigned) n);
  int sent = 0;
  for (int i = 0; i < n; i++)
    if (i != source) {
      Packet *pp = (sent < n - 2 ? p->clone() : p);
      output(i).push(pp);
      sent++;
    }
  assert(sent == n - 1);
}

void
MyHashSwitch::push(int source, Packet *p)
{
    click_ether* e = (click_ether*) p->data();
    int outport = -1;		// Broadcast

    // 0 timeout means dumb switch
    //if (_timeout != 0) {
    //	_table.set(EtherAddress(e->ether_shost), AddrInfo(source, p->timestamp_anno()));

    // Set outport if dst is unicast, we have info about it, and the
    // info is still valid.
    EtherAddress dst(e->ether_dhost);
    outport = _table[dst]; //bf.etherlookup(dst);
    //click_chatter("lookup %s %d", dst.unparse_colon().c_str(), outport);

    //if (outport < 0) {
      output(0).push(p);
      //broadcast(source, p);
      //}
      //else if (outport == source)	// Don't send back out on same interface
      //p->kill();
    //else				// forward
    //  output(outport).push(p);
}

String
MyHashSwitch::reader(Element* f, void *thunk)
{
    MyHashSwitch* sw = (MyHashSwitch*)f;
    switch ((intptr_t) thunk) {
    case 0: {
      return String();
    }
    case 1:
	return String(sw->_timeout);
    default:
	return String();
    }
}

int
MyHashSwitch::writer(const String &s, Element *e, void *, ErrorHandler *errh)
{
    MyHashSwitch *sw = (MyHashSwitch *) e;
    if (!cp_seconds_as(s, 0, &sw->_timeout))
	return errh->error("expected timeout (integer)");
    return 0;
}

void
MyHashSwitch::add_handlers()
{
    add_read_handler("table", reader, 0);
    add_read_handler("timeout", reader, (void *) 1);
    add_write_handler("timeout", writer, 0);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(MyHashSwitch)
