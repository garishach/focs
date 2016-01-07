# focs
FOCS: Fast Overlapped Community Search
version Oct-2013

This file is associated with the algorithm presented in article by Bandyopadhyay, S.; Chowdhary, G.; Sengupta, D., "FOCS: Fast Overlapped Community Search," in Knowledge and Data Engineering, IEEE Transactions on , vol.27, no.11, pp.2974-2985, Nov. 1 2015.  In case you use this software in your research, we encourage you to cite this article.


>> Disclaimer

This version runs only on LINUX operating system 

If you encounter a bug, please send corresponding report to garisha.ch@gmail.com along with the input file and parameters used (unless you know they are not required).
You can also send any comment or suggestion about the program or its legibility.

The program expects a friendly use and therefore does not exhaustively verify all arguments.


>> Input File format

The input file is in form an unweighted edgelist. Each line contains 2 numbers (node labels) separated by a tab space, representing an edge. Please check file example_dolph as an example to input file.


>> Compilation

 >to compile: gcc graphaslist_uo_main.c -lm
 >a compiled binary file is included, named 'focs'


>> Input Parameters and Running

 -KCORE: to ensure a node will have atleast KCORE connections in any community (default=2)
 -OVL: the amount(fraction) of overlap before the smaller of the communities is removed in case of near-duplicate communities (default=0.6, preferably 0.5 <= OVL <= 1)

 > Usage syntax: 
 ./a.out <input_file> KCORE OVL > output
 
 > Use either default parameters or enter both parameters.
 	example: ./focs example_dolph > output
 		  or ./focs example_dolph 2 0.5 > output
 
 
>> Output File

Each line represents a community with node labels separated by tab spaces.
The values in console output in each line starting with 'phase' represent the change in average number of communities per node across iterations (leave phases).

