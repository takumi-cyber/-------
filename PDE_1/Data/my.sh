#!/bin/bash
. draw.sh
. standards.sh
COLOR=1
X_AXIS='X'
X_RANGE='0 1 0.25'
Y_AXIS='\fIu'
Y_RANGE='-0.75 1.3 0.5'
LINE=( Dot:Red Solid:Green Solid:Blue Solid:Cyan Solid:Magenta)
SYMBOL=( Omit )

# 1時間だけ存在する一時フォルダーを作ります
TMP=`mktemp -dt myXXXXX`

# 解析解
seq 0 1|\
gawk '
function floor(x){ return int(x) - (x<0 && x!=int(x) ? 1 : 0) }
BEGIN{
    time=5;
    printf("#time%f\n",time);
    for(i=0;i<=100;i++)
    {
        x=i/100;
        xi=x-time;
        xi-=floor(xi);
        xc = xi - floor(xi+0.2) - 0.3;

        y = exp(-200*xc*xc);
        if ((xc>0.3)&&(xc<0.5)) y+=1;

        printf("%f %f\n", x, y);
    }
    printf("\n");
}' > $TMP/sol.dat


# 元のデータが粗っぽいので，ごまかします
spline my.dat > $TMP/fine.dat
# ラベルを作成します
cat > $TMP/legend.dat <<@
#m=Dot:Red,S=Omit
0.05 -0.3
0.15 -0.3 l: theory

#m=Solid:Green,S=Omit
0.05 -0.45
0.15 -0.45 l:\fINx\fR=20

#m=Solid:Blue,S=Omit
0.05 -0.6
0.15 -0.6 l:100

#m=Solid:Cyan,S=Omit
0.3 -0.6
0.4 -0.6 l:400

#m=Solid:Magenta,S=Omit
0.55 -0.6
0.65 -0.6 l:1600

#m=Solid:Black,S=Omit
0.05 1.2 t:FV Lax-Wendroff
0.05 1.05 t:CFL=0.8
0.05 0.9 t:t=5
@
LEGEND=$TMP/legend.dat
# ほんじゃ，描いてみようか
draw my.pdf $TMP/sol.dat FVLW20.dat FVLW100.dat FVLW400.dat FVLW1600.dat
