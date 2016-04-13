Documentation for Warmup Assignment 2
=====================================

+-------+
| BUILD |
+-------+

Comments: Use "make" or "make warmup2" to make and link all files and "make clean" to delete all binary and executable files.

+-----------------+
| SKIP (Optional) |
+-----------------+

Is there are any tests in the standard test suite that you know that it's not
working and you don't want the grader to run it at all so you won't get extra
deductions, please list them here.  (Of course, if the grader won't run these
tests, you will not get plus points for them.)

None.

+---------+
| GRADING |
+---------+

Basic running of the code : 100 out of 100 pts

Missing required section(s) in README file : None
Cannot compile : None
Compiler warnings : None
"make clean" : None
Segmentation faults : None
Separate compilation : Yes, Implemented.
Using busy-wait : No
Handling of commandline arguments:
    1) -n : None
    2) -lambda : None
    3) -mu : None
    4) -r : None
    5) -B : None
    6) -P : None
Trace output :
    1) regular packets: None
    2) dropped packets: None
    3) removed packets: None
    4) token arrival (dropped or not dropped):None
Statistics output :
    1) inter-arrival time : None
    2) service time : None
    3) number of customers in Q1 : None
    4) number of customers in Q2 : None
    5) number of customers at a server : None
    6) time in system : None
    7) standard deviation for time in system : None
    8) drop probability : None
Output bad format : None
Output wrong precision for statistics (should be 6-8 significant digits) : None
Large service time test : None
Large inter-arrival time test : None
Tiny inter-arrival time test : None
Tiny service time test : None
Large total number of customers test : None
Large total number of customers with high arrival rate test : None
Dropped tokens test : None
Cannot handle <Cntrl+C> at all (ignored or no statistics) : None
Can handle <Cntrl+C> but statistics way off : None
Not using condition variables and do some kind of busy-wait : None
Synchronization check : None
Deadlocks : None

+------+
| BUGS |
+------+

Comments: None

+------------------+
| OTHER (Optional) |
+------------------+

Comments on design decisions: None
Comments on deviation from spec: None
