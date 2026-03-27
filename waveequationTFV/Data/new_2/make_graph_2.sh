#!/bin/bash

. draw.sh
. standards.sh
COLOR=1
X_AXIS='\fIx'
X_RANGE='0 1 0.2'
Y_AXIS='\fIu'
Y_RANGE='-0.25 1.5 0.5'
LINE=(
  Solid:Red:0.003
  Solid:Blue:0.003
  Solid:Green:0.003
  Solid:Magenta:0.003
  Solid:Cyan:0.003
)
SYMBOL=( Omit )
cat > $TMP/legend.dat <<@
#m=Solid:Red,S=Omit
0.05 1.3 
0.1 1.3 l: t=0.00
#m=Solid:Blue,S=Omit
0.35 1.3
0.4 1.3 l: 0.25
#m=Solid:Green,S=Omit
0.6 1.3
0.65 1.3 l: 0.50
#m=Solid:Magenta,S=Omit
0.05 1.1
0.1 1.1 l: 0.75
#m=Solid:Cyan,S=Omit
0.35 1.1
0.4 1.1 l: 1.00
@
LEGEND=$TMP/legend.dat
draw step_CFL2.5_nu1000.0mu500.0_CN_CD.pdf step_CFL2.5_t0.00_nu1000.0mu500.0_CN_CD.dat step_CFL2.5_t0.25_nu1000.0mu500.0_CN_CD.dat step_CFL2.5_t0.50_nu1000.0mu500.0_CN_CD.dat step_CFL2.5_t0.75_nu1000.0mu500.0_CN_CD.dat step_CFL2.5_t1.00_nu1000.0mu500.0_CN_CD.dat
