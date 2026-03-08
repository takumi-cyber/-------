TMP=`mktemp -d -t XXX`
. draw.sh
csv(){
	cat	$1|tr , \  |gawk '{if (NR>1) printf("%e %.18e\n",$2,$3);}'
}
make_scale(){
	#make_scale scale_size x_positoon y_position
	XC=${2:-0.5}
	XF=`echo "((${XC})-0.02)"|calc -pq`
	XT=`echo "((${XC})+0.02)"|calc -pq`
	YR=( $Y_RANGE )
	YCEN=`echo "((${YR[1]})+(${YR[0]}))*0.5"|calc -pq`
	YSZ=`echo "((${YCEN}))*$1/100"|calc -pq`
	YC=${3:-${YCEN}}
	YFR=`echo "((${YC})-(${YSZ})/2)"|calc -pq`
	YTO=`echo "((${YC})+(${YSZ})/2)"|calc -pq`
	cat<<@
${XF}	${YFR}
${XT}	${YFR}

${XC}	${YFR}
${XC}	${YC}	l:\r4 $1 %
${XC}	${YTO}

${XF}	${YTO}
${XT}	${YTO}
@
}
comment(){
	sed -e '$s/$/ l: '$1'/'
}
neg(){
	gawk 'function abs(x){
		if (x<0) return -x;
		return x;
	}{
	printf("%e %.8e\n",$1,abs($2));
}'
}
cut(){
	gawk '{
	if ($1<='$1') printf("%e %.12e\n",$1,$2);
}'
}
nonzero(){
	gawk '{
	if (($1!=0)&&($2!=0)) printf("%e %.8e\n",$1,$2);
}'
}
deviation(){
	gawk '{
	printf("%e %.8e\n",$1,$2-1);
}'
}
scale(){
	gawk '{
	printf("%e %.8e\n",$1,$2/1e-4);
}'
}
set_nls(){
	grep -v ^\#	> $TMP/nls
	NAMAE=( 	`cat $TMP/nls|gawk '{printf("%s ",$1);}'` )
	LINE=(		`cat $TMP/nls|gawk '{printf("%s ",$2);}'` )
	SYMBOL=(	`cat $TMP/nls|gawk '{printf("%s ",$3);}'` )
}
#	log-scaleで描くと点線が汚いのでlogで等間隔になるように補間
log_uniform_x(){
	gawk '{printf("%le %le\n",log($1),$2);}'	>	$TMP/log_uniform
	if [ "$1" == "" ];then
		LENGTH=( `wc -l $TMP/log_uniform` )
	else
		 LENGTH=( $1 )
	fi
	spline -n ${LENGTH[0]}	<	$TMP/log_uniform	|gawk '{printf("%le %le\n",exp($1),$2)}'
}
#	time2sec 10:23:35 で秒に変換
time2sec(){
	echo $1|gawk -F : '{printf("%d",($1*60+$2)*60+$3)}'
}
#	day2sec 2024/10/31 をUNIXTIME(1970/1/1 AM9:00 (UTC0:00)以降)に変換
day2sec(){
	echo $1|gawk -F / '{printf("%d",mktime($1 " " $2 " " $3 " 0 0 0"));}'
}
#	これらはなんだ
beauty(){
	cat	$1|tr , \  |gawk '{
	if (NR>1) printf("%e %.18e\n",$2,$3/1e-4);
}'
}
delta(){
	gawk '{
	printf("%e %.8e\n",$1,$2*1e-4-1);
}'
}
