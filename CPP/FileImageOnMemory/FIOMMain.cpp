#include "FIOMMain.h"
#include <exception>
#include "cocos2d.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>

#include "FIOMNode.h"
#include "Config/TestLog.h"
#include "../../../../CreptingFiles/Translate.h"

using namespace GrizzlyJr;
using namespace std;

#define RELOAD_COUNT_MAX 3

static inline std::string& ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
static inline std::string& rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
static inline std::string& trim(std::string& s) {
	return ltrim(rtrim(s));
}

static FIOMMain::NodeFileImage loadFileStatic(MyVector<MyString>& elements, size_t& pos) {
	FIOMMain::NodeFileImage result;
	result.isArray = false;
	result.isSpecial = false;
	
	bool isBegin = false;
	char endSymbol = 0;
	if( '[' == elements.data[pos].text[0]) {
		result.isArray = true;
		result.isSpecial = false;
		endSymbol = ']';
	} else if( '<' ==elements.data[pos].text[0]) {
		result.isArray = false;
		result.isSpecial = true;
		endSymbol = '>';
	}else if( '{' == elements.data[pos].text[0]) {
		endSymbol = '}';
	}else {
		isBegin = true;
	}
	
	if( !isBegin) {
		pos++;
	}
	
	size_t size = elements.count;
	
	if( result.isArray) {
		int count = 0;
		string name = "0";
		char symbols[16] = {0};
		for( ; pos < size; pos++) {
			if( '[' == elements.data[pos].text[0] || '{' == elements.data[pos].text[0] || '<' == elements.data[pos].text[0]) {
				result.dirs[name] = loadFileStatic(elements,pos);
				count++;
				sprintf(symbols,"%d",count);
				name = symbols;
				name = trim(name);
				continue;
			}
			
			if( !isBegin && endSymbol == elements.data[pos].text[0]) {
				return result;
			}
			
			const char* texts = (char*)elements.data[pos].text;
			result.values[name] = std::string(texts,elements.data[pos].length);
			
			count++;
			sprintf(symbols,"%d",count);
			name = symbols;
			name = trim(name);
		}
		
		return result;
	}
	
	for( ; pos < size; pos++) {
		if( '=' == elements.data[pos].text[0]) {
			const char* texts = (char*)elements.data[pos-1].text;
			std::string name = std::string(texts,elements.data[pos-1].length);
			name = trim(name);
			pos++;
			
			const char* texts2 = (char*)elements.data[pos].text;
			result.values[name] = std::string(texts2,elements.data[pos].length);
			continue;
		}
		if( '[' == elements.data[pos].text[0] || '{' == elements.data[pos].text[0] || '<' == elements.data[pos].text[0]) {
			int savePos = pos-1;
			
			const char* texts = (char*)elements.data[savePos].text;
			std::string name = std::string(texts,elements.data[savePos].length);
			name = trim(name);
			result.dirs[name] = loadFileStatic(elements,pos);
			continue;
		}
		
		if( !isBegin && endSymbol == elements.data[pos].text[0]) {
			return result;
		}
	}
	
	return result;
}

FIOMMain* FIOMMain::singleton = 0x0;

FIOMNode FIOMMain::getRoot(const char* nameFile) {
	std::map<std::string,NodeFileImage>::iterator find = files.find(nameFile);
	if( find == files.end()) {
		return FIOMNode(0x0);
	}
	
	return FIOMNode(&(find->second));
}
FIOMNode FIOMMain::getNodeByPath(FilePath path) {
	if( path.count <= 0) {
		return FIOMNode(0x0);
	}
	
	FIOMNode node = getRoot(path.p[0]);
	if( !node.isCorrect()) {
		return node;
	}
	
	for( size_t i =1; i < path.count; i++) {
		node = node[path.p[i]];
	}
	return node;
}


string FIOMMain::translatePath(FilePath path) {
	string result;
	for( unsigned int i =0; i < path.count; i++) {
		result += path.p[i];
		result += "/";
	}
	return result;
}

FilePath FIOMMain::translatePath(std::string path) {
	FilePath result;
	result.count = 0;
	   
	unsigned int k =0;
	for( const char* ch = path.c_str(); ; ch++) {
		if( 0 ==  *ch) {
			if( k > 0) {
				result.count++;
				k = 0;
			}
			break;
		}

		if( '\\' == *ch || '/' == *ch) {
			if( k > 0) {
				result.count++;
				k = 0;
			}
			continue;
		}

		result.p[result.count][k] = *ch;
		result.p[result.count][k+1] = 0;
		k++;
		}

	   return result;
}

FIOMMain::FIOMMain() {
	singleton = this;
	crepting.setKey("grizzly-jr-crepting-key");
}

FIOMMain* FIOMMain::get() {
	if( 0x0 == singleton) {
		return new FIOMMain();
	}
	return singleton;
}

bool FIOMMain::isPath(FilePath path) {
	if( 0x0 == getInfo(path).node) {
		return false;
	}
	return true;
}

FIOMMain::NodeFileImageResult FIOMMain::getNode(std::string pathStr) {
	FilePath path = translatePath(pathStr);
	if( path.count == 0) {
		NodeFileImageResult result;
		result.node = 0x0;
		return result;
	}
	path.count--;
	return getInfo(path);
}
std::string FIOMMain::getValue(std::string pathStr) {
	FilePath path = translatePath(pathStr);
	if( path.count == 0) {
		return "";
	}
	return path.p[path.count-1];
}

FIOMMain::NodeFileImageResult FIOMMain::getInfo(FilePath path) {
	NodeFileImageResult result;
	result.node = 0x0;
	if( path.count <= 0) {
		return result;//"Incorrect FilePath file."
	}
	map<string,NodeFileImage>::iterator iter = files.find(path.p[0]);
	if( files.end() == iter ) {
		return result;//"Don't find file."
	}

	for( unsigned int i =1; i < path.count;i++) {
		map<string,NodeFileImage>::iterator next = iter->second.dirs.find(path.p[i]);
		if( next == iter->second.dirs.end() ) {
			return result;//"Don't find node."
		}

		iter = next;
	}

	result.path = path;
	result.node = &(iter->second);
	return result;
}

FIOMMain::NodeFileImageResult FIOMMain::getInfo(const NodeFileImageResult& node, const char* name) {
	NodeFileImageResult result;
	result.node = node.node;
	result.path = node.path;
	if( 0x0 == node.node) {
		return result;
	}
	
	map<string,NodeFileImage>::iterator iter = node.node->dirs.find(name);
	if( iter == result.node->dirs.end()) {
		result.node = 0x0;
		return result;
	}
	
	result.path.addPath(name);
	result.node = &(iter->second);
	return result;
}

std::string FIOMMain::getStr(const NodeFileImageResult& node, const char* name)const {
	if( 0x0 == node.node) {
		return "";
	}
	if( node.node->values.end() != node.node->values.find(name) ){
		return node.node->values[name];
	}
	return "";
	
}
void FIOMMain::setStr(const NodeFileImageResult& node, const char* name, std::string value) {
	node.node->values[name] = value;
}

std::string FIOMMain::getNameFromPath(std::string pathStr)
{
	FilePath path = translatePath(pathStr);
	if( 0 == path.count) {
		return "";
	}
	
	char* result = path.p[path.count-1];
	
	return result;//names.p[names.count-1];
}

std::string FIOMMain::loadFile(std::string pathStr) {
	closeFile(pathStr);
	string fileName = getNameFromPath(pathStr);

	for( size_t reload_i = 0; reload_i < RELOAD_COUNT_MAX; reload_i++) {	
		unsigned char* dataFile = 0x0;
		unsigned long sizeFile = 0;
		

		cocos2d::CCFileUtils::sharedFileUtils()->setPopupNotify(false);
	#ifdef REPLACE_SESSION_MODE
		const char* pathChar = cocos2d::CCFileUtils::fullPathFromRelativePath(pathStr.c_str());
		
		dataFile = cocos2d::CCFileUtils::getFileData(pathChar,"r",&sizeFile);
		CCAssert(dataFile, "Don't find file constant - "+pathStr);
	#else 
		std::string filePath = cocos2d::CCFileUtils::sharedFileUtils()->getWriteablePath()+fileName;
		dataFile = cocos2d::CCFileUtils::sharedFileUtils()->getFileData(filePath.c_str(),"r",&sizeFile);
		if( 0x0 == dataFile) {
			const char* pathChar = cocos2d::CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(pathStr.c_str());
			dataFile = cocos2d::CCFileUtils::sharedFileUtils()->getFileData(pathChar,"r",&sizeFile);
			
			std::string msg = "Get data from file("+fileName+") failed!";
	#ifdef DEBUG_FILE_MODE
			if( 0x0 == dataFile) {
				cocos2d::CCMessageBox(msg.c_str(), "Notification");
			}
	#else
			CCAssert(dataFile, "Don't find file constant - "+pathStr);
	#endif
		}
	#endif
		cocos2d::CCFileUtils::sharedFileUtils()->setPopupNotify(true);

		bool isLoad = loadFileData(fileName, dataFile, sizeFile);
		delete[] dataFile;
		if( isLoad) {
			return fileName;
		}
		
	}
	return fileName;
}

static void print_fileDIR(FIOMMain::NodeFileImage& dir,int tabs =0) {
	std::map<std::string,FIOMMain::NodeFileImage>::iterator iter;
	std::string tabs_str;
	for( int i =0; i < tabs; i++) {
		tabs_str += "--";
	}
	
	for( iter = dir.dirs.begin(); iter != dir.dirs.end(); ++iter) {
		cocos2d::CCLog("%s",(tabs_str+iter->first).c_str());
		print_fileDIR(iter->second,tabs+1);
	}
}

bool FIOMMain::loadFileData(std::string fileName,unsigned char* dataFile, unsigned long sizeFile) {
	if( 0x0 == dataFile || sizeFile < ADD_SIZE_FOR_DECRYPT_SYSTEM) {
		return false;
	}
	std::string input = crepting.decryptMe((const char*)dataFile+ADD_SIZE_FOR_DECRYPT_SYSTEM,sizeFile-ADD_SIZE_FOR_DECRYPT_SYSTEM);
	
	MyVector<MyString> elements;
	MyString temp;
	temp.text = (unsigned char*)input.c_str();
	temp.length = input.size();
	translate(temp,elements);
	if( !test_for_correct(dataFile,input,elements)) {
		TestLog(input.c_str());
		return false;
	}
	
	
	if( elements.count > 0) {
		size_t i = 0;
		files[fileName] = loadFileStatic(elements,i);
		cocos2d::CCLog("load file - %s:",fileName.c_str());
	} else {
		files[fileName] = NodeFileImage();
		cocos2d::CCLog("don't load file - %s. size =%ld",fileName.c_str(),sizeFile);
	}
	
	for( size_t i =0; i < elements.count; i++) {
		free(elements.data[i].text);
	}
	
	return true;
}

bool FIOMMain::isLoad(std::string path) {
	string fileName = getNameFromPath(path);
	
	if( files.end() == files.find(fileName) ) {
		return false;
	}
	return true;
}

static bool test_NODE(const FIOMMain::NodeFileImage& node) {
	if( 0 == node.dirs.size() + node.values.size() ) {
		return true;
	}
	return false;
}

int FIOMMain::saveFile(const NodeFileImage& node, std::string& output,bool isFirst) {
	if( test_NODE(node)) {
		return 0;
	}
	size_t node_count = 0;
	
	if( node.isArray && !isFirst) {
		output += "[ "; node_count++;
		
		for( unsigned int i=0; i < node.dirs.size()+node.values.size(); i++) {
			char number[32] = {0};
			sprintf(number,"%d",i);

			std::map<std::string,NodeFileImage>::const_iterator iter = node.dirs.find(number);
			if( iter != node.dirs.end() ) {
				node_count += saveFile(iter->second,output,false);
				continue;
			}
			std::map<std::string,std::string>::const_iterator iterV = node.values.find(number);
			if( iterV != node.values.end() ) {
				output += " \""+iterV->second+"\" "; node_count++;
				continue;
			}			
		}

		output += "] "; node_count++;
	} else {
		if( !isFirst) {
			output += "{ "; node_count++;
		}
		
		std::map<std::string,NodeFileImage>::const_iterator iter;
		for( iter = node.dirs.begin(); iter != node.dirs.end(); iter++) {
			if( !test_NODE(iter->second)) {
				output += iter->first; node_count++;
				node_count += saveFile(iter->second,output,false);
			}
		}
		std::map<std::string,std::string>::const_iterator iterV;
		for( iterV = node.values.begin(); iterV != node.values.end(); iterV++) {
			if( !iterV->first.empty() && !iterV->second.empty()) {
				output += iterV->first; node_count++;
				output += " = "; node_count++;
				output += "\""+iterV->second+"\" "; node_count++;
			}
		}
		
		if( !isFirst) {
			output += "} "; node_count++;
		}
	}
	
	return node_count;
}

void FIOMMain::closeFile(std::string path)  {
	string fileName = getNameFromPath(path);
	
	std::map<std::string,NodeFileImage>::iterator iter = files.find(fileName);
	if( files.end() == iter ) {
		return;
	}
	
	//cocos2d::CCLog("close file:%s",fileName.c_str());
	files.erase(iter);
}

void FIOMMain::saveFile(std::string pathStr)  {
	std::string fileName = getNameFromPath(pathStr);
	FilePath path = translatePath(fileName);
	std::string filePath = cocos2d::CCFileUtils::sharedFileUtils()->getWriteablePath()+fileName;
	
	NodeFileImage* node = getInfo(path).node;
	if( node) {
		NodeFileImage nodeSave = *node;
		std::string output;
		size_t node_count = saveFile(nodeSave, output,true);
		size_t file_size = output.size();
		output = crepting.encryptMe(output);
		addSizeInString(output,node_count);
		addSizeInString(output,file_size);
		
		FILE* file = fopen(filePath.c_str(), "w");
		if( 0x0 != file) {
			fprintf(file, "%s",output.c_str());
			fclose(file);
		} else {
			cocos2d::CCLog("don't save file - %s.",fileName.c_str());
		}
	}
}