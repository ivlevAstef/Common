#!/bin/bash
clear

TP=/usr/local/bin/TexturePacker
DIR=`pwd`
SOURCE_PATH="/Users/stef/Dropbox/CuteHeroes/PROJECT"
RESULT_PATH="/Users/stef/projects/git/CuteHeroesCross/CuteHeroes/Resources/frame-cache"
DOWNLOAD_ADDITION_NAME="-download"

##SOURCE_PATH="/Users/stef/projects/purpleTexture/units"
##RESULT_PATH="/Users/stef/projects/purpleTexture/frame-cache"

#$DIR"/zwoptex"

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
declare -a addition_name_resource=(
""
""
""
""
##  ""
##	""
##	""
)

infoSize=3

echoTexturePackerInfo()
{
	log="$3"
	if [ "" == "$log" ]; then
		log="Error"
	fi
	echo "EXECUTE $log $1$2"
}

createTexture()
{
	local fullpath=$1
	local addition_name=$2
	local realpath=${fullpath/$SOURCE_PATH/""}
	local fname=${fullpath##*/}
	
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

	isNoRotation=`ls $1/no_rot 2> /dev/null`
	rotationAtribute="--enable-rotation"
	if [ -n "$isNoRotation" ]; then
		rotationAtribute="--disable-rotation"
	fi

	formatTestFile=`ls $1/format 2> /dev/null`
	formatOption="--opt RGBA8888"
	formatFile=".png"
	if [ "" != "$formatTestFile" ]; then
		declare -a arrayF
		old_IFS=$IFS
		IFS=$'\n'
		arrayF=( `cat "$1/format"`)
		formatOption="${arrayF[0]}"
		formatFile="${arrayF[1]}"
		IFS=$old_IFS
	fi


	declare -r -i countPostfix=$((${#postfixInfo[*]} / $infoSize))
	result=0
	for((i=0; i < $countPostfix; ++i)); do
		postfixName=${postfixInfo[i*$infoSize +0]}
		postfixScale=${postfixInfo[i*$infoSize +1]}
		postfixMax=${postfixInfo[i*$infoSize +2]}
		local additional_local_name=${addition_name_resource[i]}

		if [ -z "$additional_local_name" ]; then
			additional_local_name=$addition_name
		fi
		isDowRes=`ls $1/dow_res_$postfixName 2> /dev/null`
		if [ -n "$isDowRes" ]; then
			additional_local_name=$DOWNLOAD_ADDITION_NAME
		fi

		resultFullPath="$RESULT_PATH$additional_local_name/$postfixName$realpath"
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

		testFiles=`ls $fullpath/*.png 2> /dev/null`
		if [ -n "$testFiles" ]; then
			mkdir -p $resultFullPath
		
			echoTexturePackerInfo "POSTFIX $postfixName PATH $realpath" "$additional_local_name" $(${TP} 2> /dev/null --smart-update --scale $postfixScale $pathPacket/*.png \
			--max-width $postfixMax \
			--max-height $postfixMax \
			--trim \
			--allow-free-size \
			$rotationAtribute \
			--padding 0\
			$formatOption\
			--shape-padding 2\
			--format cocos2d \
			--data $resultFullPath/$resultFileName.plist \
			--sheet $resultFullPath/$resultFileName$formatFile)
			result=1

			testFiles=`ls $resultFullPath/*.png 2> /dev/null`
			if [ -n "$testFiles" ]; then
				`imageOptim -d $resultFullPath -q`
			fi
			
		fi

	done

	return $result
}

recursion()
{
	local addition_name=$2
	isDownloadResource=`ls $1/dow_res 2> /dev/null`
	if [ -n "$isDownloadResource" ]; then
		addition_name=$DOWNLOAD_ADDITION_NAME
	fi

	createTexture $1 "$addition_name"

	if [ $? == 1 ]; then
		return
	fi

	for nameF in $1/*; do
		if [ -d $nameF ]; then
			recursion $nameF "$addition_name"
		fi
	done
}

if [ "$1" ]; then
	USE_ONLY_REALITY_PATH=$1
fi

`export PATH=$PATH:`
recursion "$SOURCE_PATH" ""
