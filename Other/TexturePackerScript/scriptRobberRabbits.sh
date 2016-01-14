#!/bin/bash
clear

TP=/usr/local/bin/TexturePacker
DIR=`pwd`
SOURCE_PATH="/Users/stef/Dropbox/RRAndroid/Graphics"
RESULT_PATH="/Users/stef/Dropbox/RRAndroid/temp"

USE_ONLY_REALITY_PATH=""


declare -a postfixInfoNormal=(
	"normal" 0.41667 1024
	"hd" 0.83333 2048
	"ipad" 1 2048
	"android1" 0.625 2048
	"android2" 0.78125 2048
	"android3" 1.041667 2048
	"android4" 0.9375 2048
)

declare -a postfixInfoIpadHd=(
"normal" 0.20834 1024
"hd" 0.41667 2048
"ipad" 0.5 2048
"ipad-hd" 1 4096
"android1" 0.3125 2048
"android2" 0.390625 2048
"android3" 0.520834 2048
"android4" 0.46875 2048
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


	testFiles=`ls $1/no_hd 2> /dev/null`
	if [ "" == "$testFiles" ]; then
		postfixInfo=()
		for item in "${postfixInfoIpadHd[@]}"; do
			postfixInfo[${#postfixInfo[@]}]=$item
		done
	fi

	testFiles=`ls $1/postfix 2> /dev/null`
	if [ "" != "$testFiles" ]; then
		postfixInfo=()
		declare -a array1
		array1=( `cat "$1/postfix" | tr '\n' ' '`)

		for item in "${array1[@]}"; do
			postfixInfo[${#postfixInfo[@]}]=$item
		done
	fi

	isNoRotation=`ls $1/no_rot 2> /dev/null`

	declare -r -i countPostfix=$((${#postfixInfo[*]} / $infoSize))
	result=0
	for((i=0; i < $countPostfix; ++i)); do
		postfixName=${postfixInfo[i*$infoSize +0]}
		postfixScale=${postfixInfo[i*$infoSize +1]}
		postfixMax=${postfixInfo[i*$infoSize +2]}

		resultFullPath="$RESULT_PATH/$postfixName$realpath"
		resultFileName="$fname-$postfixName"
		
		if [ "$2" ]; then
			mkdir -p $resultFullPath

			if [ "" == "$isNoRotation" ]; then
				echoTexturePackerInfo "POSTFIX $postfixName PATH $realpath ROTATION=YES" $(${TP} 2> /dev/null --smart-update --scale $postfixScale $fullpath/*.png \
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
			else
				echoTexturePackerInfo "POSTFIX $postfixName PATH $realpath ROTATION=NO" $(${TP} 2> /dev/null --smart-update --scale $postfixScale $fullpath/*.png \
				--max-width $postfixMax \
				--max-height $postfixMax \
				--trim \
				--allow-free-size \
				--disable-rotation \
				--padding 2\
				--format cocos2d \
				--data $resultFullPath/$resultFileName.plist \
				--sheet $resultFullPath/$resultFileName.png) 
				result=1
			fi

		else
			testDirectories=`ls $fullpath/$postfixName 2> /dev/null`
			if [ "" != "$testDirectories" ]; then
				mkdir -p $resultFullPath

				if [ "" == "$isNoRotation" ]; then
					echoTexturePackerInfo "POSTFIX $postfixName PATH $realpath ROTATION=YES" $(${TP} 2> /dev/null --smart-update $fullpath/$postfixName/*.png \
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
					echoTexturePackerInfo "POSTFIX $postfixName PATH $realpath ROTATION=NO" $(${TP} 2> /dev/null --smart-update $fullpath/$postfixName/*.png \
					--max-width $postfixMax \
					--max-height $postfixMax \
					--trim \
					--allow-free-size \
					--disable-rotation \
					--padding 0\
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
	testFiles=`ls $1/*.png 2> /dev/null`
	if [ "" != "$testFiles" ]; then
		createTexture $1 "true"
	else
		createTexture $1
	fi

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
