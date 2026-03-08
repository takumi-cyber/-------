#!/bin/bash

. draw.sh
. standards.sh
COLOR=1
X_AXIS='\fIx'
X_RANGE='0 1 0.2'
Y_AXIS='\fIu'
Y_RANGE='-0.25 1.5 0.5'
LINE=( Solid:Red:0.003 Solid:Black:0.01 Dash:7:0.004)
SYMBOL=( Omit )
OPT="--pen-colors=1=DarkRed:2=MediumSeaGreen:3=Blue:4=Magenta:5=GoldenRod3:6=Black:7=Gray100 --symbol-line-width 0.0020"
cat > $TMP/legend.dat <<@
#m=Solid:Red,S=Omit
0.05 1.40
0.10 1.40 l: traditional(CFL=0.8)

#m=Solid:Black,S=Omit
0.05 1.25 
0.10 1.25 l: theory

#m=Dash:Black,S=Omit
0.05 1.10
0.13 1.10 l: present(CFL=12.5)

@
LEGEND=$TMP/legend.dat
draw step.pdf step_CFL0.8_traditional_t1.00.dat u_t1.00.dat step_CFL12.5_t1.00.dat
