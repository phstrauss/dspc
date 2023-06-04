This is an pure C "port" of the Differential Evolution solver code by Lester Godwin, originaly in C++,
with parent-centric mutation/crossover, solution domain bounds enforcement (clipping) by myself,
plus an OCaml binding to it.

DE is the 4 wheel driving of solver, in short, random probing, genetic inspired.
C Port and adds-on parent-centric mutation, plus OCaml binding by Philippe Strauss,
<philippe at strauss-acoustics.ch>, Jan 2012.


Original code in .scaffolding/LEG.cpp/, author note:

// Differential Evolution Solver Class
// Based on algorithms developed by Dr. Rainer Storn & Kenneth Price
// Written By: Lester E. Godwin
//             PushCorp, Inc.
//             Dallas, Texas
//             972-840-0208 x102
//             godwin@pushcorp.com
// Created: 6/8/98
// Last Modified: 6/8/98
// Revision: 1.0
