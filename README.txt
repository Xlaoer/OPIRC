This is a public-domain implementation of OPIRC. 
OPIRC is an Output-Interleaved Range Coding Algorithm, published in Data Compression Conference 2025
"OPIRC: An Output-Interleaved Range Coding Algorithm" (https://ieeexplore.ieee.org/document/10992327).

OPIRC is a parallel range coding method that employs multiple encoders and decoders without the need for the swizzling step. It is achieved by designing a sliding window mechanism to interleave the outputs of multiple encoders, so that the positions of each encoder’s outputs in the bitstream follow a predictable and ordered pattern.This design saves bandwidth, reduces encoding latency, and enables each decoder to pre-locate the data it needs to read during decoding, thereby improving both compression and decompression performance.

Here is the paper description and outcome variables comparison:
Plain      -> RC mentioned in the paper
OPIRC-2way -> 2-way_Ours mentioned in the paper
OPIRC-3way -> 3-way_Ours mentioned in the paper
2LANE      -> 2-way FVRC mentioned in the paper
3LANE      -> 3-way FVRC mentioned in the paper
----

You can use `cmake . && make` command to build the executable file in different directories.

Results on my machine (Intel(R) Core(TM) i9-12900H CPU) with 8 GB main memory on Ubuntu 20.04:

----

Here's run 5 times OPIRC-2way

----
Reading file ../plant/clementin.fasta
plain encoding  249.1 MiB/sec.
plain decoding  159.0 MiB/sec.
plain Compressed file from 512437 bytes to 167647 bytes
OPIRC-2way encoding 266.9 MiB/sec.
OPIRC-2way decoding  235.6 MiB/sec.
OPIRC-2way Compressed file from 512437 bytes to 167644 bytes
Decompression OK

Reading file ../plant/clementin.fasta
plain encoding  100.6 MiB/sec.
plain decoding  73.4 MiB/sec.
plain Compressed file from 512437 bytes to 167647 bytes
OPIRC-2way encoding 255.6 MiB/sec.
OPIRC-2way decoding  139.5 MiB/sec.
OPIRC-2way Compressed file from 512437 bytes to 167644 bytes
Decompression OK

Reading file ../plant/clementin.fasta
plain encoding  82.3 MiB/sec.
plain decoding  80.2 MiB/sec.
plain Compressed file from 512437 bytes to 167647 bytes
OPIRC-2way encoding 157.2 MiB/sec.
OPIRC-2way decoding  175.5 MiB/sec.
OPIRC-2way Compressed file from 512437 bytes to 167644 bytes
Decompression OK

Reading file ../plant/clementin.fasta
plain encoding  105.7 MiB/sec.
plain decoding  92.6 MiB/sec.
plain Compressed file from 512437 bytes to 167647 bytes
OPIRC-2way encoding 253.2 MiB/sec.
OPIRC-2way decoding  227.4 MiB/sec.
OPIRC-2way Compressed file from 512437 bytes to 167644 bytes
Decompression OK

Reading file ../plant/clementin.fasta
plain encoding  98.6 MiB/sec.
plain decoding  122.6 MiB/sec.
plain Compressed file from 512437 bytes to 167647 bytes
OPIRC-2way encoding 248.7 MiB/sec.
OPIRC-2way decoding  238.0 MiB/sec.
OPIRC-2way Compressed file from 512437 bytes to 167644 bytes
Decompression OK
----

Here's run 5 times OPIRC-3way

----
Reading file ../plant/clementin.fasta
plain encoding  96.6 MiB/sec.
plain decoding  97.5 MiB/sec.
plain Compressed file from 512437 bytes to 167647 bytes
OPIRC-3way encoding 195.2 MiB/sec.
OPIRC-3way decoding  272.6 MiB/sec.
OPIRC-3way Compressed file from 512437 bytes to 167655 bytes
Decompression OK

Reading file ../plant/clementin.fasta
plain encoding  84.3 MiB/sec.
plain decoding  78.0 MiB/sec.
plain Compressed file from 512437 bytes to 167647 bytes
OPIRC-3way encoding 223.3 MiB/sec.
OPIRC-3way decoding  248.6 MiB/sec.
OPIRC-3way Compressed file from 512437 bytes to 167655 bytes
Decompression OK

Reading file ../plant/clementin.fasta
plain encoding  101.5 MiB/sec.
plain decoding  118.5 MiB/sec.
plain Compressed file from 512437 bytes to 167647 bytes
OPIRC-3way encoding 163.7 MiB/sec.
OPIRC-3way decoding  279.7 MiB/sec.
OPIRC-3way Compressed file from 512437 bytes to 167655 bytes
Decompression OK

Reading file ../plant/clementin.fasta
plain encoding  67.9 MiB/sec.
plain decoding  89.2 MiB/sec.
plain Compressed file from 512437 bytes to 167647 bytes
OPIRC-3way encoding 239.7 MiB/sec.
OPIRC-3way decoding  222.8 MiB/sec.
OPIRC-3way Compressed file from 512437 bytes to 167655 bytes
Decompression OK

Reading file ../plant/clementin.fasta
plain encoding  109.7 MiB/sec.
plain decoding  91.6 MiB/sec.
plain Compressed file from 512437 bytes to 167647 bytes
OPIRC-3way encoding 195.2 MiB/sec.
OPIRC-3way decoding  250.9 MiB/sec.
OPIRC-3way Compressed file from 512437 bytes to 167655 bytes
Decompression OK
----

Here's run 5 times 2LANE

----
Reading file ../plant/clementin.fasta
plain encoding  126.1 MiB/sec.
plain decoding  138.4 MiB/sec.
plain Compressed file from 512437 bytes to 167647 bytes, 100.289% vs. theoretical limit
2LANE encoding 159.3 MiB/sec.
2LANE Compressed file from 512437 bytes to 167648 bytes
2LANE decoding 243.4 MiB/sec.
Decompression OK

Reading file ../plant/clementin.fasta
plain encoding  220.0 MiB/sec.
plain decoding  151.4 MiB/sec.
plain Compressed file from 512437 bytes to 167647 bytes, 100.289% vs. theoretical limit
2LANE encoding 170.8 MiB/sec.
2LANE Compressed file from 512437 bytes to 167648 bytes
2LANE decoding 247.9 MiB/sec.
Decompression OK

Reading file ../plant/clementin.fasta
plain encoding  86.0 MiB/sec.
plain decoding  76.7 MiB/sec.
plain Compressed file from 512437 bytes to 167647 bytes, 100.289% vs. theoretical limit
2LANE encoding 88.7 MiB/sec.
2LANE Compressed file from 512437 bytes to 167648 bytes
2LANE decoding 245.9 MiB/sec.
Decompression OK

Reading file ../plant/clementin.fasta
plain encoding  99.0 MiB/sec.
plain decoding  121.0 MiB/sec.
plain Compressed file from 512437 bytes to 167647 bytes, 100.289% vs. theoretical limit
2LANE encoding 171.3 MiB/sec.
2LANE Compressed file from 512437 bytes to 167648 bytes
2LANE decoding 245.2 MiB/sec.
Decompression OK

Reading file ../plant/clementin.fasta
plain encoding  231.5 MiB/sec.
plain decoding  160.4 MiB/sec.
plain Compressed file from 512437 bytes to 167647 bytes, 100.289% vs. theoretical limit
2LANE encoding 160.6 MiB/sec.
2LANE Compressed file from 512437 bytes to 167648 bytes
2LANE decoding 247.6 MiB/sec.
Decompression OK
----

Here's run 5 times 3LANE

----
Reading file ../plant/clementin.fasta
plain encoding  89.4 MiB/sec.
plain decoding  83.0 MiB/sec.
plain Compressed file from 512437 bytes to 167647 bytes, 100.289% vs. theoretical limit
3LANE encoding 96.2 MiB/sec.
3LANE Compressed file from 512437 bytes to 167658 bytes
3LANE decoding 245.9 MiB/sec.
Decompression OK

Reading file ../plant/clementin.fasta
plain encoding  91.7 MiB/sec.
plain decoding  97.2 MiB/sec.
plain Compressed file from 512437 bytes to 167647 bytes, 100.289% vs. theoretical limit
3LANE encoding 143.9 MiB/sec.
3LANE Compressed file from 512437 bytes to 167658 bytes
3LANE decoding 240.1 MiB/sec.
Decompression OK

Reading file ../plant/clementin.fasta
plain encoding  141.0 MiB/sec.
plain decoding  133.9 MiB/sec.
plain Compressed file from 512437 bytes to 167647 bytes, 100.289% vs. theoretical limit
3LANE encoding 147.1 MiB/sec.
3LANE Compressed file from 512437 bytes to 167658 bytes
3LANE decoding 236.8 MiB/sec.
Decompression OK

Reading file ../plant/clementin.fasta
plain encoding  109.6 MiB/sec.
plain decoding  101.1 MiB/sec.
plain Compressed file from 512437 bytes to 167647 bytes, 100.289% vs. theoretical limit
3LANE encoding 152.3 MiB/sec.
3LANE Compressed file from 512437 bytes to 167658 bytes
3LANE decoding 238.6 MiB/sec.
Decompression OK

Reading file ../plant/clementin.fasta
plain encoding  92.3 MiB/sec.
plain decoding  116.7 MiB/sec.
plain Compressed file from 512437 bytes to 167647 bytes, 100.289% vs. theoretical limit
3LANE encoding 146.4 MiB/sec.
3LANE Compressed file from 512437 bytes to 167658 bytes
3LANE decoding 245.1 MiB/sec.
Decompression OK
----
