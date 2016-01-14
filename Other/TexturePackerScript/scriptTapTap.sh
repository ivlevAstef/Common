#!/bin/bash
clear

TP=/usr/local/bin/TexturePacker
DIR=`pwd`
SOURCE_PATH="/Users/stef/Dropbox/demo/PROJECT"
RESULT_PATH="/Users/stef/projects/git/TapTap/TapTAp/Resources/graphics"
#$DIR"/zwoptex"

USE_ONLY_REALITY_PATH="" ##in command string


declare -a postfixInfoNormal=(
	"normal" 0.41667 1024
	"hd" 0.83333 2048
	"ipad" 1 2048
##  "android1" 0.625 2048
##	"android2"  2048
##	"android3"  2048
)

infoSize=3

echoTexturePackerInfo()
{
	log="$2"
	if [ "" == "$log" ]; then
		log="Error"
	fi
	echo "EXECUTE $log $1"
}

createTexture()
{
	fullpath=$1
	realpath=${fullpath/$SOURCE_PATH/""}
	fname=${fullpath##*/}
	
	if [ "" != "$USE_ONLY_REALITY_PATH" ]; then
		len=`expr "$realpath" : "$USE_ONLY_REALITY_PATH"`
		if [ 0 == $len ]; then
			return 0
		fi
	fi

	postfixInfo=()
	for item in "${postfixInfoNormal[@]}"; do
		postfixInfo[${#postfixInfo[@]}]=$item
	done

	testFiles=`ls $1/postfix 2> /dev/null`
	if [ "" != "$testFiles" ]; then
		postfixInfo=()
		declare -a array1
		array1=( `cat "$1/postfix" | tr '\n' ' '`)

		for item in "${array1[@]}"; do
			postfixInfo[${#postfixInfo[@]}]=$item
		done
	fi

	declare -r -i countPostfix=$((${#postfixInfo[*]} / $infoSize))
	result=0
	for((i=0; i < $countPostfix; ++i)); do
		postfixName=${postfixInfo[i*$infoSize +0]}
		postfixScale=${postfixInfo[i*$infoSize +1]}
		postfixMax=${postfixInfo[i*$infoSize +2]}

		resultFullPath="$RESULT_PATH/$postfixName$realpath"
		resultFileName="$fname-$postfixName"
		
		testDirectories=`ls $fullpath/$postfixName 2> /dev/null`

		if [ "" != "$testDirectories" ]; then
			mkdir -p $resultFullPath

			echoTexturePackerInfo "POSTFIX $postfixName PATH $realpath" $(${TP} 2> /dev/null --smart-update $fullpath/$postfixName/*.png \
			--max-width $postfixMax \
			--max-height $postfixMax \
			--trim \
			--allow-free-size \
			--enable-rotation \
			--padding 0\
			--format cocos2d \
			--data $resultFullPath/$resultFileName.plist \
			--sheet $resultFullPath/$resultFileName.png)
			result=1
		else
			testDirectories=`ls $fullpath/hd 2> /dev/null`
			if [ "normal" == "$postfixName" -a "" != "$testDirectories" ]; then
				mkdir -p $resultFullPath

				echoTexturePackerInfo "POSTFIX $postfixName PATH $realpath" $(${TP} 2> /dev/null --smart-update --scale 0.5 $fullpath/hd/*.png \
				--max-width $postfixMax \
				--max-height $postfixMax \
				--trim \
				--allow-free-size \
				--enable-rotation \
				--padding 0\
				--format cocos2d \
				--data $resultFullPath/$resultFileName.plist \
				--sheet $resultFullPath/$resultFileName.png)
				result=1
			else
				testFiles=`ls $fullpath/*.png 2> /dev/null`
				if [ "" != "$testFiles" ]; then
					mkdir -p $resultFullPath

					echoTexturePackerInfo "POSTFIX $postfixName PATH $realpath" $(${TP} 2> /dev/null --smart-update --scale $postfixScale $fullpath/*.png \
					--max-width $postfixMax \
					--max-height $postfixMax \
					--trim \
					--allow-free-size \
					--enable-rotation \
					--padding 2\
					--format cocos2d \
					--data $resultFullPath/$resultFileName.plist \
					--sheet $resultFullPath/$resultFileName.png) 
					result=1
				fi
			fi

		fi
	done

	return $result
}

recursion()
{
	createTexture $1

	if [ $? == 1 ]; then
		return
	fi

	for nameF in $1/*; do
		if [ -d $nameF ]; then
			recursion $nameF
		fi
	done
}

if [ "$1" ]; then
	USE_ONLY_REALITY_PATH=$1
fi

recursion "$SOURCE_PATH"
