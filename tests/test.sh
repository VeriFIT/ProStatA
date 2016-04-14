#!/bin/bash
SAVEIFS=$IFS
IFS=$(echo -en "\n\b")

REF_PATH="."
LOG_PATH="./tmp"
PASS="../passes_build/libpasses.so"
#PASS="../passes/libpasses.so"
CLANG=clang

ALL=0
GOOD=0

if [ $# -eq 1 ]; then
	if [ $1 = "clean" ]; then
		rm -r "$LOG_PATH"
		exit 0
	fi
fi

if [ ! -x $PASS ]; then
	echo $0": not execute permission for user: $PASS" >&2
	exit 1
fi

mkdir -p "$LOG_PATH"

ps=$(find $REF_PATH -maxdepth 1 -type d |  sed -n "s/^\.\/\(.*\)$/\1/p")
for opt in $ps; do
	if [ $opt = "tmp" ]; then
		continue;
	fi

	echo -e "\033[0;36m==$opt================================================\033[0m"
	mkdir -p "$LOG_PATH/$opt"
	OPATH="$LOG_PATH/$opt"

	testcase=$(ls "$REF_PATH/$opt/"*.c | sed -n "s/^.*\/\(.*\)\.c$/\1/p")
	for i in $testcase; do

		RESULT="OK"
		EC=0

#		${CLANG} -S -emit-llvm --std=c99 "$REF_PATH/$opt/$i.c" -o "$OPATH/$i.ll"
#		${CLANG} "$OPATH/$i.ll" -o "$OPATH/a.out"
		${CLANG} --std=c99 "$REF_PATH/$opt/$i.c" -o "$OPATH/a.out"
		EC=$(( $? == 0 ? EC : $? ))
		./$OPATH/a.out > "$OPATH/$i.out" 2> "$OPATH/$i.err"
		ECA=$?

		TMPIFS=$IFS
		IFS=$SAVEIFS
		${CLANG} -S -emit-llvm --std=c99 "$REF_PATH/$opt/$i.c" -o - | opt -S -load "$PASS" -${opt} -o "$OPATH/$i-pass.ll" 2> /dev/null
		EC=$(( $? == 0 ? EC : $? ))
		IFS=$TMPIFS

		${CLANG} "$OPATH/$i-pass.ll" -o "$OPATH/a.out"
		EC=$(( $? == 0 ? EC : $? ))
		./$OPATH/a.out > "$OPATH/$i-pass.out" 2> "$OPATH/$i-pass.err"
		ECB=$?

		if [ $EC -ne 0 ]; then
			RESULT="BAD"
		fi

		if [ $ECA -ne $ECB ]; then
			RESULT="BAD EC"
		fi
		
		diff -up "$OPATH/$i.out" "$OPATH/$i-pass.out" > "$OPATH/$i.out-delta"
		if [ $? -ne 0 ] || [ ! -e "$OPATH/$i.out-delta" ] || [ -s "$OPATH/$i.out-delta" ]
			then   # diff neuspesne, neexist vysledok, nenulova dlzka
				RESULT="BAD OUT"
		fi

		diff -up "$OPATH/$i.err" "$OPATH/$i-pass.err" > "$OPATH/$i.err-delta"
		if [ $? -ne 0 ] || [ ! -e "$OPATH/$i.err-delta" ] || [ -s "$OPATH/$i.err-delta" ]
			then   # diff neuspesne, neexist vysledok, nenulova dlzka
				RESULT="BAD ERR"
		fi

		ALL=$((1+ALL))
		if [ "$RESULT" = "OK" ]; then
			GOOD=$((1+GOOD))
			echo -e "$i: \033[0;32m$RESULT\033[0m"
		else
			echo -e "$i: \033[0;31m$RESULT\033[0m"
		fi

#		if [ "$RESULT" = "BAD OUT" ]; then
#			echo "------------------------"
#			cat "$LOG_PATH/$i.out-delta"
#			echo "------------------------"
#		fi
	done
done

# vysledok
	echo -e "\033[0;36m==========================================================\033[0m"
	GOOD=$((GOOD*100/ALL))
	echo "correct: $GOOD%"

IFS=$SAVEIFS
exit 0

