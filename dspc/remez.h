/**************************************************************************
 * Parks-McClellan algorithm for FIR filter design (C version)
 *-------------------------------------------------
 *  Copyright (c) 1995,1998  Jake Janovetz (janovetz@uiuc.edu)
 *  PST notice: current email (2010): nospam@janovetz.com (source: google)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.

 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *************************************************************************/


#ifndef __REMEZ_H__
#define __REMEZ_H__

#ifdef __cplusplus
extern "C" {
#endif


#define STANDARD       0
#define DIFFERENTIATOR 1
#define HILBERT        2

#define ODD            0
#define EVEN           1

#define Pi             3.1415926535897932
#define Pi2            6.2831853071795865

#define GRIDDENSITY    16
#define MAXITERATIONS  80

// #define RET_CONVERGED	1
#define RET_FAILED		0

/* flexible API */
int remez_from_grid (
   double h[],
   int numtaps,
   double Grid[],
   double D[],
   double W[],
   int gridlen,
   int type,
   double epsilon);

/* common API */
int remez_from_bands (
   double h[],
   int numtaps,
   int numband,
   double transitions[],
   double des[],
   double weight[],
   int type,
   double epsilon);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

