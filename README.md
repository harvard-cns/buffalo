Programs in this release:


lsfib: simulate packet stretch in a network with shortest path routing
- I studied three networks: ISP, dc (data center), and campus
- INPUT: topology. see conext/1239.bof for the input topology file for the ISP
network. (We cannot release the other two topologies)

BUFFALO switch prototype in bfclick:
- use the ip directory in bfclick to replace the
click/elements/ip directory..

buffalo switch bloom filter simulation: bloomtest
- use Bloom filter to construct a FIB based on a given FIB
- use packet trace (a list of dst MAC addresses) to simulate
the false positives for the Bloom filter construction