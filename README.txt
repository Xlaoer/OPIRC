This is an official implementation of OPIRC. OPIRC is an
Output-Interleaved Range Coding Algorithm, as described in Na Wang's paper
"OPIRC: An Output-Interleaved Range Coding Algorithm" (https://ieeexplore.ieee.org/document/10992327).
If you have any questions about this work or would like to explore collaboration opportunities, please don’t hesitate to contact the authors directly:
- wna@usst.edu.cn
- sjhenglin@gmail.com

Here is the paper description and outcome variables comparison:
Plain -> paper's RC
OPIRC-2way -> paper's 2-way_Ours
OPIRC-3way -> paper's 3-way_Ours
2LANE -> paper's 2-way FVRC
3LANE -> paper's 3-way FVRC
----

Results on my machine (Intel(R) Core(TM) i9-12900H CPU) with 8 GB main memory on Ubuntu 20.04:

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
