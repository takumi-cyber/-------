#!/bin/bash
#汎用人型決戦スクリプト R1.10	graphがめんどいので, だいたいなんでも描けるスクリプト
#編集してはいけません!これは実行ファイル自身です. 今すぐエディターを閉じなさい
#---
$Fri Jun  6 10:17:26 JST 2025	1.10	バグ訂正
#Fri Jul 31 10:35:14 JST 2020	1.09	マーカー白抜, set_color関数
#Fri Sep  8 19:24:11 JST 2017	1.08	NEWPLOTで図の数が正確でないこと路を訂正
#Fri Jan 20 12:04:44 JST 2017	1.07	モノクログラフでOPT変数が効かないバグを訂正
#Mon Jan 16 14:02:21 JST 2017	1.06	OSX Serria対応
#Mon Sep 26 16:12:17 JST 2016	1.05	LINES変数は, 通常の環境変数と混じるのでLINEに変更
#Fri Sep 23 11:11:53 JST 2016	1.04	LineColor Solid:Black:0.002 形式を追加
#Thu Sep 22 16:03:35 JST 2016	1.03	NEWPLOTバグを訂正
#	average(), av_SM(), get_MV()を追加
#Sun Jun  5 13:46:46 JST 2016	1.00	マルチ図の線幅を訂正
#Sat Jun  4 14:16:17 JST 2016	0.91	色を文字で指定しましょう
#Sat Jun  4 11:36:27 JST 2016	0.90	初めてのバージョン
#
#---
ASPECT=${ASPECT:-0.6}
DUMB=${DUMB:-0}
FONT_NAME=${FONT_NAME:-'Times-Roman'}
FONT_SIZE=${FONT_SIZE:-0.075}
LABEL_FONT_NAME=${LABEL_FONT_NAME:-$FONT_NAME}
LABEL_FONT_SIZE=${LABEL_FONT_SIZE:-$FONT_SIZE}
TITLE_FONT_NAME=${TITLE_FONT_NAME:-$FONT_NAME}
TITLE_FONT_SIZE=${TITLE_FONT_SIZE:-$FONT_SIZE}
SYMBOL_SIZE=${SYMBOL_SIZE:-0.05}
LINE_WIDTH=${LINE_WIDTH:-0.003}
GRID_STYLE=${GRID_STYLE:-2}
OPT=""
UNIVERSAL_LEGEND=${UNIVERSAL_LEGEND:-0}
GRID_LINE_WIDTH=${GRID_LINE_WIDTH:-$LINE_WIDTH}
MARKER_WHITE=white
COLOR=0
REMOVE_ERRBAR=0
#MacOSのbash V3では連想配列が使えない
cid(){
case $1 in
"Red")		echo 1;;
"Green")	echo 2;;
"Blue")		echo 3;;
"Magenta")	echo 4;;
"Cyan")		echo 5;;
*)			echo 6;;
esac
}
set_color(){
	#set_color red new_color
	_cid=`cid $1`
	COLOR_MAP[$_cid]=$2
}
set_color Red Red
set_color Green Green
set_color Blue Blue
set_color Magenta Magenta
set_color Cyan Cyan
set_color Black Black
#
check(){
	FAIL=0
	type gawk > /dev/null
	if [ $? -ne 0 ];then
		echo "gawk not found."
		FAIL=1
	fi
	type graph > /dev/null
	if [ $? -ne 0 ];then
		echo "PlotUtil not found."
		FAIL=1
	fi
	type ps2pdf > /dev/null
	if [ $? -ne 0 ];then
		echo "ps2pdf not found."
		FAIL=1
	fi
	echo $FAIL
}
add(){
	gawk '{
	printf("%f %f\n",$1,$2+('$1'))
}'
}
randomize(){
	gawk 'BEGIN{
	_C='$1';
	srand('$RANDOM');
}{
	printf("%e %e\n",$1,$2*(1+_C*rand()));
}'
}
_least_2(){
	gawk 'BEGIN{
		sum=0;sumx=0;sumxx=0;sumy=0;sumxy=0;
	}{
		sum=sum+1;
		sumx=sumx+$1;
		sumxx=sumxx+$1*$1;
		sumy=sumy+$2;
		sumxy=sumxy+$1*$2;
	}END{
		a=(sumxy*sum-sumx*sumy)/(sumxx*sum-sumx*sumx);
		b=(-a*sumx+sumy)/sum;
		printf("%e %e\n",a,b);
	}'
}
least_2(){
	#xyの最小自乗直線を導きます. outputは, y=ax+b のaとbです.
	#LS_RANGE変数が定義されていると, outputはa,bではなく,
	#LS_RANGEで指定された関数値になります
	#least_2 < input > output
	if [ -z $LS_RANGE ];then
		_least_2;
	else
		_least_2|gawk '{
			x='${LS_RANGE[0]}';	printf("%e %e\n",x,$1*x+$2);
			x='${LS_RANGE[1]}';	printf("%e %e\n",x,$1*x+$2);
		}'
	fi
}
average(){
	#yの平均値を計算します
	#average < input > output
	gawk 'BEGIN{
		nx=0;sumy=0;
	}{
		nx=nx+1;sumy=sumy+$2;
	}END{
		printf("%f\n",sumy/nx);
	}'
}
av_SM(){
	#Simple Moving Averageを実行します.
	#av_SM N [STEP] < input  > output
	#av_SMは, X Y データを受け取ります. N行のデータを読み込むまでは出力しません.
	#N行以降のデータを読み取ると, 直近N行の X Yの平均値を出力します.
	#STEPが指定された場合, STEP行に1回だけ出力します
	step=${2:-"1"}
	gawk 'BEGIN{
		average='$1';step='$step'; sumx=0; sumy=0;
	}{
		ci=(NR-1)%average;
		if ( NR >= average ) {
			sumx=sumx+($1-x[ci]);
			sumy=sumy+($2-y[ci]);
			x[ci]=$1;
			y[ci]=$2;
			if ( NR%step == 0 ) {
				printf("%e %e\n",sumx/average,sumy/average);
			}
		} else {
			sumx=sumx+$1;
			sumy=sumy+$2;
			x[ci]=$1;
			y[ci]=$2;
		}
	}'
}
get_MV(){
	#値が変動したときだけ, 値を出力する
	#get_MV < input > output
	gawk '{
		if ( NR == 1 ) {
			y=$2;
		} else {
			diff=y-$2;
			if ( diff < 0 ) diff=-diff;
			if ( diff > 1e-10) {
				printf("%e %e\n",$1,$2);
				y=$2;
			}
		}
	}'
}
swap(){
	gawk '{printf("%e %e\n",$2,$1)}'
}
get_pair(){
	gawk '{printf("%e %e\n",$'$1',$'$2')}' $3
}
line_select(){
	IFS=: read -ra _arg <<< "$1"
	_style=${_arg[0]}	#	Solid Dash ...
	_color=${_arg[1]}	#	Black Red ...
	_width=${_arg[2]}	#	0.005 0.006 ...
	case $_color in
	Red) _color=1;;
	1) _color=1;;
	Green) _color=2;;
	2) _color=2;;
	Blue) _color=3;;
	3) _color=3;;
	Magenta) _color=4;;
	4) _color=4;;
	Cyan) _color=5;;
	5) _color=5;;
	Black) _color=6;;
	6) _color=6;;
	*);;
	esac
	if [ $COLOR -eq 0 ];then _color=1;fi
	case $_style in
	Omit) echo 0;return;;
	Solid) _style=1;;
	Dot) _style=2;;
	DotDash) _style=3;;
	Dash) _style=4;;
	LongDash) _style=5;;
	DotDotDash) _style=6;;
	DotDotDotDash) _style=7;;
	*) echo $1;return;;
	esac
	(( RES = $_color + ( $_style - 1 ) * $COLORS )) 
	if [ "$_width" != "" ];then
		echo "$RES --line-width $_width"
	else
		echo "$RES"
	fi
}
symbol_size(){
	IFS=: read -ra _arg <<< "$1"
	_style=${_arg[0]}	#	FilledCircle
	_color=${_arg[1]}	#	Black Red ...
	_width=${_arg[2]}	#	0.005 0.006 ...
	if [ "$_width" != "" ];then
		echo $_width
	else
		echo $2
	fi
}
symbol_color(){
	if [ $COLOR -eq 0 ];then echo 0;return;fi
	IFS=: read -ra _arg <<< "$1"
	_style=${_arg[0]}	#	FilledCircle
	_color=${_arg[1]}	#	Black Red ...
	_width=${_arg[2]}	#	0.005 0.006 ...
	case $_color in
	Red) echo -1;;
	1) echo -1;;
	Green) echo -2;;
	2) echo -2;;
	Blue) echo -3;;
	3) echo -3;;
	Magenta) echo -4;;
	4) echo -4;;
	Cyan) echo -5;;
	5) echo -5;;
	Black) echo -6;;
	6) echo -6;;
	*) echo -6;;
	esac
}
symbol_select(){
	IFS=: read -ra _arg <<< "$1"
	_style=${_arg[0]}	#	FilledCircle
	_color=${_arg[1]}	#	Black Red ...
	_width=${_arg[2]}	#	0.005 0.006 ...
	case $_style in
	Omit) echo 255;;
	Dot) echo 1;;
	Plus) echo 2;;
	Star) echo 3;;
	HollowCircle) echo 4;;
	Cross) echo 5;;
	HollowSquare) echo 6;;
	HollowDelta) echo 7;;
	HollowDiamond) echo 8;;
	HollowNabla) echo 10;;
	PlusTarget) echo 12;;
	CrossTarget) echo 13;;
	Lockon) echo 14;;
	Target) echo 15;;
	FilledCircle|Circle) echo 16;;
	FilledSquare|Square) echo 17;;
	FilledDelta|Delta) echo 18;;
	FilledDiamond|Diamond) echo 19;;
	FilledNabla|Nabla) echo 20;;
	WhiteCircle) echo 23;;
	WhiteSquare) echo 24;;
	WhiteDelta) echo 25;;
	WhiteDiamond) echo 26;;
	WhiteNabla) echo 27;;
	*) echo $_style;;
	esac
}
#現状, 凡例で設定できるのは, 線種, マーカーだけで, 色と大きさが制御できてない
make_legend()
{
	MYTMP=`mktemp -t drawXXXX`
	cat $1|while read -r line
	do
		case "$line" in
		\#m=*)	
			_SYMB=`expr $line : '.*S=\(.*\)' '|' $line`
			_SMOD=`symbol_select $_SYMB`
			if [ "$_SMOD" -ne 255 -a "$_SMOD" -ne 0 ];then
				#色をS=...から採用
				_LMOD=`symbol_color $_SYMB`
			else
				#S=Omitであるので色はm=...から採用
				_LINE=`expr $line : '#m=\(.*\),' '|' $line`
				_LMOD_=( `line_select $_LINE` )
				_LMOD=${_LMOD_[0]}
			fi
			printf "#m=%s,S=%s\n" $_LMOD $_SMOD	>>	$MYTMP
			;;
		*)
			printf "%s\n" "$line"	>>	$MYTMP
			;;
		esac
	done
	if [ $UNIVERSAL_LEGEND -eq 1 ];then
		echo "--reposition 0 0 1 --blankout 0 -g 0 -y 0 1 -Y '' -x 0 1 -X '' -I L -m 0 -S 0 $LEGEND_OPT $MYTMP"
	else
		echo "-I L -m 1 -S 0 $LEGEND_OPT $MYTMP"
	fi
}
graph_gray()
{
case "$1" in
red)	COLOR="1 0 0";;
white)	COLOR="1 1 1";;
black)	COLOR="0 0 0";;
green)	COLOR="0 1 0";;
blue)	COLOR="0 0 1";;
*)	COLOR="$1";;
esac
sed\
	-e 's/[0]*.[1-9][0-9]* [0]*.[1-9][0-9]* [0]*.[1-9][0-9]* SetCBg/'"$COLOR"' SetCBg/g'\
	-e 's/0.750000 SetP/1 SetP/g'\
	-e 's/0.500000 SetP/1 SetP/g'\
	-e 's/0.250000 SetP/1 SetP/g'\

}
draw(){
	#色設定は影響が大きいので最初に行う
	if [ $COLOR -ne 0 ];then
		COLORS=7
		PEN_COLOR="--pen-colors="
		PEN_COLOR="${PEN_COLOR}1=${COLOR_MAP[1]}:2=${COLOR_MAP[2]}"
		PEN_COLOR="${PEN_COLOR}:3=${COLOR_MAP[3]}:4=${COLOR_MAP[4]}"
		PEN_COLOR="${PEN_COLOR}:5=${COLOR_MAP[5]}:6=${COLOR_MAP[6]}"
		_OPT[0]="-C $PEN_COLOR $OPT"
	else
		COLORS=1
		_OPT[0]="$OPT"
	fi
	#NEWPLOT指定がなければ, $# - 2 が図ファイルの数
	if [ -z $NEWPLOT ]; then (( newplot=$# - 1 ));else newplot=(${NEWPLOT[*]});fi
	PLOTS=${#newplot[*]}
	#最初のコマンド
	_X_RANGE=${X_RANGE[0]:-"0 1 0.25"}
	_Y_RANGE=${Y_RANGE[0]:-"-1 1 0.5"}
	CMD="graph -B -Tps\
	-h $ASPECT -I L\
	--grid-style ${GRID_STYLE[0]}\
	--font-name "${FONT_NAME[0]}" --font-size ${FONT_SIZE[0]}\
	--label-font-name "${LABEL_FONT_NAME[0]}" --label-font-size ${LABEL_FONT_SIZE[0]}\
	--title-font-name "${TITLE_FONT_NAME[0]}" --title-font-size ${TITLE_FONT_SIZE[0]}\
	--frame-line-width ${LINE_WIDTH[0]} --line-width ${LINE_WIDTH[0]}\
	-n y -y $_Y_RANGE -Y '${Y_AXIS[0]}'\
	-n x -x $_X_RANGE -X '${X_AXIS[0]}'\
	${_OPT[0]}\
	"
	#図を描く
	DONE=0
	id=1
	for (( plot=0 ; plot < $PLOTS ; plot++ ))
	do
		(( NFIG=${newplot[$plot]} - DONE ))
		for (( i=0 ; i < $NFIG ; i++ ))
		do
			(( id = id + 1 ))
			_F="\${${id}}"
			FILE=`eval echo $_F`
			if [ -z "$FILE" ]; then break;fi
			if [ ! -f "$FILE" ]; then echo "File[$FILE] not found.";break;fi
			if [ $plot -gt 0 -a $i -eq 0 ];then
				#新しい図の開始である
				_POS=${POSITION[$plot]:-"0 0 1"}
				__POS=($_POS)
				_LW=${LINE_WIDTH[0]}
				_LW=`echo "scale=10;$_LW/${__POS[2]}"|bc`
				_X_RANGE=${X_RANGE[$plot]:-${X_RANGE[0]}}
				_Y_RANGE=${Y_RANGE[$plot]:-${Y_RANGE[0]}}
				_X_AXIS=${X_AXIS[$plot]:-""}
				_Y_AXIS=${Y_AXIS[$plot]:-""}
				_FONT_SIZE=${FONT_SIZE[$plot]:-${FONT_SIZE[0]}}
				_LW=${LINE_WIDTH[$plot]:-$_LW}
				CMD="$CMD\
					--reposition $_POS\
					--blankout 0 --toggle-axis-end y\
					--frame-line-width $_LW --line-width $_LW\
					--font-size $_FONT_SIZE\
					-y $_Y_RANGE -Y '$_Y_AXIS'\
					-x $_X_RANGE -X '$_X_AXIS'\
					${OPT[$plot]}\
					"
			fi
			(( _def_line = DONE % 5 ))
			_line=${LINE[$DONE]:-`expr $DONE % 8`}
			_symbol=${SYMBOL[$DONE]:-"Omit"}
			symbol=`symbol_select ${_symbol}`
			color=`symbol_color ${_symbol}`
			size=`symbol_size ${_symbol} $SYMBOL_SIZE`
			line=`line_select ${_line}`
			if [ $symbol -le 0 ];then
				ADD="-m $line -S 0 $_F"
			else
				case "$REMOVE_ERRBAR" in
				2|line)
					#最初に線を引き, エラーバーをOFFにしてからシンボルを描き, 元に戻す
					ADD="-m $line -S 0 --toggle-ignore-errorbar $_F --toggle-ignore-errorbar -m $color -S $symbol $size $_F"
					;;
				1|symbol)
					#最初に線を引き, エラーバーをOFFにしてからシンボルを描き, 元に戻す
					ADD="-m $line -S 0 $_F --toggle-ignore-errorbar -m $color -S $symbol $size $_F --toggle-ignore-errorbar"
					;;
				*)
					#最初に線を引き, 次にシンボルを描く
					ADD="-m $line -S 0 $_F -m $color -S $symbol $size $_F"
				esac
			fi
			if [ $DUMB -ne 1 ]; then
				echo -e "$plot/$PLOTS[$_X_RANGE][$_Y_RANGE]$i\c"
				printf "[%-10.10s|%-10.10s]" $_line $_symbol
				if [ ${#FILE} -gt 30 ];then
					echo "...${FILE:$((${#FILE}-30))}"
				else
					echo "${FILE}"
				fi
			fi
			(( DONE = DONE + 1 ))
			CMD="$CMD $ADD"
		done
		if [ ! -z "${LEGEND[$plot]}" ];then
			CMD="$CMD `make_legend ${LEGEND[$plot]}`"
		fi
	done
	cat > draw.log <<@
---graph command---
$CMD

@
	eval $CMD | graph_gray $MARKER_WHITE | tee log.eps |  ps2pdf -dEPSCrop - 	$1
}

if [ `check`  -ne 0 ];then
	exit
fi
#　使用例を作成なんちゃって
if [ "$1" == "dump"  ];then
	zip draw.zip $0
	echo "DRAW.ZIP"
	echo "std::string ZIP_CODE;{"
	od -A n -vt x1 draw.zip | tr -Cd '0-9a-f\n' | sed -e 's/^/ZIP_CODE+="/' -e 's/$/";/'
	echo "}"
fi
if [ "$1" == "init"  ];then
	if [ ! -f my.dat ];then
		echo "Creating my.dat"
		gawk 'END{
			pi=atan2(0,-1); N=5;
			for(i=0;i<=N;i++){
				t=i/N;
				X=0.8*sin(2*pi*t);
				printf("%f %f\n",t,X);
			}
		}' < /dev/null > my.dat
	fi
	if [ ! -f my.sh ];then
		echo "Creating my.sh"
		cat>my.sh <<@
#!/bin/bash
. draw.sh

X_AXIS='my X-axis'
X_RANGE='0 1 0.25'
Y_AXIS='my Y-axis'
Y_RANGE='-1 1 0.5'
LINE=( Solid:Black Omit:Black Solid:Blue )
SYMBOL=( Omit WhiteCircle:Red Omit )

# 1時間だけ存在する一時フォルダーを作ります
TMP=\`mktemp -dt myXXXXX\`
# 元のデータが粗っぽいので，ごまかします
spline my.dat > \$TMP/fine.dat
# ラベルを作成します
cat > \$TMP/legend.dat <<@
#m=Solid:Black,S=Omit
0.5 0.75
0.6 0.75 l: theory
#m=Omit,S=WhiteCircle:Red
0.55 0.6 l:\r2\r4 experiment
@
	echo @>>my.sh
	cat>>my.sh <<@
LEGEND=\$TMP/legend.dat
# ほんじゃ，描いてみようか
draw my.pdf \$TMP/fine.dat my.dat
@
		chmod +x my.sh
	fi
fi
