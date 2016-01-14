#!/bin/bash
clear

TP=/usr/local/bin/TexturePacker
DIR=`pwd`
SOURCE_PATH="/Users/stef/Dropbox/RRAndroid/GraphicBearsIOS"
RESULT_PATH="/Users/stef/projects/git/RobberBears/RobberBears/Resources/graphics"

USE_ONLY_REALITY_PATH="" ##in command string


declare -a postfixInfoNormal=(
	"normal" 0.208 1024
	"hd" 0.41667 2048
	"ipad" 0.5 2048
	"ipad-hd" 1 4096
##  "android1" 0.625 2048
##	"android2"  2048
##	"android3"  2048
)

declare -a postfixInfoNoHD=(
"normal" 0.41667 1024
"hd" 0.83 2048
"ipad" 1 2048
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

	isNoHD=`ls $1/no_hd 2> /dev/null`
	if [ -n "$isNoHD" ]; then
		for item in "${postfixInfoNoHD[@]}"; do
			postfixInfo[${#postfixInfo[@]}]=$item
		done
	else
		for item in "${postfixInfoNormal[@]}"; do
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
	rotationAtribute="--enable-rotation"
	if [ -n "$isNoRotation" ]; then
		rotationAtribute="--disable-rotation"
	fi


	declare -r -i countPostfix=$((${#postfixInfo[*]} / $infoSize))
	result=0
	for((i=0; i < $countPostfix; ++i)); do
		postfixName=${postfixInfo[i*$infoSize +0]}
		postfixScale=${postfixInfo[i*$infoSize +1]}
		postfixMax=${postfixInfo[i*$infoSize +2]}

		resultFullPath="$RESULT_PATH/$postfixName$realpath"
		resultFileName="$fname-$postfixName"

		pathPacket=$fullpath

		testDirectories=`ls $fullpath/$postfixName 2> /dev/null`
		if [ -n "$testDirectories" ]; then
			pathPacket=$fullpath/$postfixName
			postfixScale=1
		else
			testDirectories=`ls $fullpath/hd 2> /dev/null`
			if [ "normal" == "$postfixName" -a -n "$testDirectories" ]; then
				pathPacket=$fullpath/hd
				postfixScale=0.5
			fi
		fi

		testFiles=`ls $pathPacket/*.png 2> /dev/null`
		if [ -n "$testFiles" ]; then
			mkdir -p $resultFullPath
		
			echoTexturePackerInfo "POSTFIX $postfixName PATH $realpath" $(${TP} 2> /dev/null --smart-update --scale $postfixScale $pathPacket/*.png \
			--max-width $postfixMax \
			--max-height $postfixMax \
			--trim \
			--allow-free-size \
			$rotationAtribute \
			--padding 0\
			--shape-padding 2\
			--format cocos2d \
			--data $resultFullPath/$resultFileName.plist \
			--sheet $resultFullPath/$resultFileName.png)
			result=1
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
