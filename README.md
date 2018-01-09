# illumio-test-repo
<<<<<<< Updated upstream


1.     how you tested your solution
  I wrote a FLOW and a NAT file, and I tested them against my program. A more rigorous way to test my program would be to write a script that generates NATs and IPs, but for such a simple program I didn't do that.

2.     any interesting coding, design, or algorithmic choices you’d like to point out
  I chose to make a class to hold an IP address, and a class to hold a NAT entry. Then, I could hash the NAT_entries by their "from" IP, and so I had a very quick and easy lookup to see if an IP from our FLOW had hashed into an IP in out NAT. Because of the possibility of hash conflicts, I also gave each IP a key. Since an IP is less than 64 bits, I just used the ip:port values as the key.
  
3.     any refinements or optimizations that you would’ve implemented if you had more time
  I differentiated between IPs with/without splats poorly, I would have used some of the spare bits in the "unsigned long key" as flags for IP splats and Port splats. Then, when I compared to see if the keys were equal while I was checking to see if the IPs that hashed the same were actually the same, I coulda used a function "compareKeys()" that took into account the flags, and acted accordingly. (If a flag for ipSplat was set, then only compare the bits of the key pertaining to the port)

4.     how you modified or made the challenge more well defined if you did
  I assumed that all input would be valind/well-formed, and that there'd be exactly one entry per line for each file. That was stated in the prompt, but I feel the need to restate it. Another assumption I made, was that no splats would be in the FLOW file.
  
5.     anything else you’d like the reviewer to know
  All of the coding challenges that I've done before have been on https://www.hackerrank.com/, it's a pretty sweet site, and I think they make distributing questions like this really easy. Unless, of course, you were also testing me on my ability to use Git, then using HackerRank to distribute the question kinda takes away from that.