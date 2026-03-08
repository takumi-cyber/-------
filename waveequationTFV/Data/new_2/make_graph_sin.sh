#!/bin/bash

. draw.sh
. standards.sh
COLOR=1
X_AXIS='\fIx'
X_RANGE='0 1 0.2'
Y_AXIS='\fIu'
Y_RANGE='-4.0 5.5 1.0'
LINE=( Solid:Black:0.01 Dash:7:0.004)
SYMBOL=( Omit )
OPT="--pen-colors=1=DarkRed:2=MediumSeaGreen:3=Blue:4=Magenta:5=GoldenRod3:6=Black:7=Gray100 --symbol-line-width 0.0020"
cat > $TMP/legend.dat <<@

#m=Solid:Black,S=Omit
0.05 4.80 
0.10 4.80 l: theory

#m=Dash:Black,S=Omit
0.05 4.30
0.13 4.30 l: present(CFL=12.5)

@
LEGEND=$TMP/legend.dat
draw sin2.pdf u_t1.00_sin_exact_nu5.0mu10.0.dat sin_CFL12.5_t1.00_nu5.0mu10.0.dat
