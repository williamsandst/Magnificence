## Magnificence - Chess Engine written in C++, ELO rating of ~2000  
The Magnificence Chess Engine uses UCI (Universal Chess Interface) as communication standard, which means that it is compatible with the majority of Chess GUI Software. The engine testing has mainly been done through [Arena Chess GUI](http://www.playwitharena.de/). The engine has an estimated ELO of 2000, but is not tested thoroughly as the hashing is currently somewhat buggy. 

## Technologies utilized  
The board is represented through bitboards, generally seen as the most efficient board representation. This can be seen by the perft speeds of 180 million legal moves/second generated on a testing computer (with no hashing or multithreading, which brings it up to 1.8 billion moves/second).    
The search uses Alpha Beta with Move Sorting, Transposition Tables (hashing), as well as other standard chess engine techniques. It does minimal pruning and no reductions. Late Move Reductions was a planned feature.  

## Credits
Developed by Harald Bjurulf and William Sandström during 2017/2018.
