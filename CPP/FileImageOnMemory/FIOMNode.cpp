//
//  FIOMNode.cpp
//  CuteHeroes
//
//  Created by Alexander Ivlev on 06.08.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "FIOMNode.h"
#include "FilePath.h"

using namespace GrizzlyJr;
using namespace cocos2d;
using namespace std;

static char* itoa(unsigned int value) {
	static char result[128] = {0};
	
	char* ptr = result;
	char* ptr1 = result;
	char tmp_char = 0x0;
	unsigned int tmp_value = 0;
	
	do {
		tmp_value = value;
		value /= 10;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value*10)];
	} while ( value );
	
	///REVERSE
	*ptr-- = '\0';
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}

FIOMNode FIOMNode::get(const char* name) {
	return FIOMMain::get()->getRoot(name);
}

FIOMNode::FIOMNode(FIOMMain::NodeFileImage* node) {
	this->node = node;
}

FIOMNode::FIOMNode(const FIOMNode& node) {
	this->node = node.node;
}

const FIOMNode& FIOMNode::operator=(const FIOMNode& copy) {
	this->node = copy.node;
	return *this;
}

size_t FIOMNode::getArraySize() {
	if( !isCorrect()) {
		return 0;
	}
	
	return node->dirs.size()+ node->values.size();
}

void FIOMNode::resize(size_t newSize) {
	if( !isCorrect()) {
		return; 
	}
	
	std::map<std::string,FIOMMain::NodeFileImage>::iterator iter = node->dirs.begin();
	while(iter != node->dirs.end()) {
		int number = 0;
		sscanf(iter->first.c_str(),"%d",&number);
		if( number >= (int)newSize) {
			node->dirs.erase(iter);
			iter = node->dirs.begin();
			continue;
		}
		iter++;
	}
	
	std::map<std::string,std::string>::iterator iter2 = node->values.begin();
	while(iter2 != node->values.end()) {
		int number = 0;
		sscanf(iter2->first.c_str(),"%d",&number);
		if( number >= (int)newSize) {
			node->values.erase(iter2);
			iter2 = node->values.begin();
			continue;
		}
		iter2++;
	}
	
}

std::vector<std::string> FIOMNode::getCatalogNames() {	
	std::vector<std::string> result;
	if( !isCorrect()) {
		return result;
	}
	
	result.resize(node->dirs.size());
	std::map<std::string,FIOMMain::NodeFileImage>::iterator iter;
	size_t i =0;
	for( iter = node->dirs.begin(); iter != node->dirs.end();iter++,i++) {
		result[i] = iter->first;
	}
	
	return result;
}

std::vector<std::string> FIOMNode::getValueNames() {
	std::vector<std::string> result;
	if( !isCorrect()) {
		return result;
	}
	
	result.resize(node->values.size());
	std::map<std::string,std::string>::iterator iter;
	size_t i =0;
	for( iter = node->values.begin(); iter != node->values.end();iter++, i++) {
		result[i] = iter->first;
	}
	
	return result;
}

FIOMNode FIOMNode::addNextDepth(unsigned int number,bool isArray,bool isSpecial)const {
	return addNextDepth(itoa(number), isArray, isSpecial);
}
FIOMNode FIOMNode::addNextDepth(std::string str,bool isArray,bool isSpecial)const {
	return addNextDepth(str.c_str(), isArray, isSpecial);
}

FIOMNode FIOMNode::addNextDepth(const char* name,bool isArray,bool isSpecial)const {
	if( !isCorrect() || 0x0 == name) {
		return FIOMNode(0x0);
	}
	
	std::map<std::string,FIOMMain::NodeFileImage>::iterator find = node->dirs.find(name);
	if( node->dirs.end() == find) {
		FIOMMain::NodeFileImage newNode;
		newNode.isArray = isArray;
		newNode.isSpecial = isSpecial;
		node->dirs[name] = newNode;
	}
	return this->operator[](name);
}

FIOMNode FIOMNode::addNextDepth(unsigned int number,FIOMNode node)const {
	return addNextDepth(itoa(number), node);
}
FIOMNode FIOMNode::addNextDepth(std::string str,FIOMNode node)const {
	return addNextDepth(str.c_str(), node);
}
FIOMNode FIOMNode::addNextDepth(const char* name,FIOMNode newNode)const {
	if( !isCorrect() || 0x0 == name) {
		return FIOMNode(0x0);
	}
	
	std::map<std::string,FIOMMain::NodeFileImage>::iterator find = node->dirs.find(name);
	if( node->dirs.end() == find) {
		node->dirs[name] = *newNode.node;
	}
	return this->operator[](name);
}

FIOMNode FIOMNode::set(unsigned int number,FIOMNode node)const {
	return set(itoa(number), node);
}
FIOMNode FIOMNode::set(std::string str,FIOMNode node)const {
	return set(str.c_str(), node);
}
FIOMNode FIOMNode::set(const char* name,FIOMNode newNode)const {
	if( !isCorrect() || 0x0 == name) {
		return FIOMNode(0x0);
	}
	
	node->dirs[name] = *newNode.node;
	return this->operator[](name);
}

FIOMNode FIOMNode::clear() {
	if( !isCorrect()) {
		return *this;
	}
	
	node->dirs.clear();
	node->values.clear();
	return *this;
}

FIOMNode FIOMNode::operator[](const char* name)const {
	if( !isCorrect() || 0x0 == name) {
		return *this;
	}
	
	std::map<std::string,FIOMMain::NodeFileImage>::iterator find = node->dirs.find(name);
	if( node->dirs.end() == find) {
		return FIOMNode(0x0);
	}
	return FIOMNode(&find->second);
}

FIOMNode FIOMNode::operator[](std::string name)const {
	return this->operator[](name.c_str());
}

FIOMNode FIOMNode::operator[](unsigned int number)const {
	return this->operator[](itoa(number));
}

bool FIOMNode::is(const char* name) {
	if( !isCorrect() || 0x0 == name) {
		return false;
	}
	
	std::map<std::string,std::string>::iterator find = node->values.find(name);
	if( node->values.end() == find) {
		std::map<std::string,FIOMMain::NodeFileImage>::iterator find = node->dirs.find(name);
		return (node->dirs.end() != find);
	}
	return true;
}
bool FIOMNode::is(std::string name) {
	return is(name.c_str());
}
bool FIOMNode::is(unsigned int number) {
	return is(itoa(number));
}

void FIOMNode::set(const char* name,std::string value) {
	if( !isCorrect() || 0x0 == name) {
		return;
	}
	node->values[name] = value;
}

void FIOMNode::set(const char* name,const char* value) {
	set(name, std::string(value));
	return;
}
	
void FIOMNode::set(const char* name,int value) {
	char per[100] = {0};
	sprintf(per, "%d",value);
	set(name, std::string(per));
	return;
}

void FIOMNode::set(const char* name,unsigned int value) {
	char per[100] = {0};
	sprintf(per, "%d",value);
	set(name, std::string(per));
	return;
}

void FIOMNode::set(const char* name,unsigned long long int value) {
	char per[100] = {0};
	sprintf(per, "%lld",value);
	set(name, std::string(per));
	return;
}
	
void FIOMNode::set(const char* name,float value) {
	char per[100] = {0};
	sprintf(per, "%f",value);
	set(name, std::string(per));
	return;
}
	
void FIOMNode::set(const char* name,bool value) {
	if( true == value) {
		set(name, std::string("true"));
		return;
	}
	set(name, std::string("false"));
	return;
}
	
void FIOMNode::set(const char* name,cocos2d::CCPoint value) {
	if( !isCorrect() || 0x0 == name) {
		return;
	}
	
	FIOMNode node = addNextDepth(name, false, false);
	node.set("x", value.x);
	node.set("y", value.y);
}
	
void FIOMNode::set(const char* name,cocos2d::CCSize value) {
	FIOMNode node = addNextDepth(name, false, false);
	node.set("width", value.width);
	node.set("height", value.height);
}
	
void FIOMNode::set(const char* name,cocos2d::CCRect value) {
	FIOMNode node = addNextDepth(name, false, false);
	node.set("x", value.origin.x);
	node.set("y", value.origin.y);
	node.set("width", value.size.width);
	node.set("height", value.size.height);
}
	
void FIOMNode::set(const char* name,cocos2d::ccColor3B value) {
	FIOMNode node = addNextDepth(name, false, false);
	node.set("r", (int)value.r);
	node.set("g", (int)value.g);
	node.set("b", (int)value.b);
}


std::string FIOMNode::getStr(const char* name) {
	if( !isCorrect() || 0x0 == name) {
		return "";
	}
	
	std::map<std::string,std::string>::iterator find = node->values.find(name);
	if( find == node->values.end()) {
		return "";
	}
	return find->second;
}

int FIOMNode::getInt(const char* name) {
	string str = getStr(name);
	int result = 0;
	sscanf(str.c_str(),"%d",&result);
	return result;
}

unsigned long long int FIOMNode::getULInt(const char* name) {
	string str = getStr(name);
	unsigned long long int result = 0;
	sscanf(str.c_str(),"%lld",&result);
	return result;
}

unsigned int FIOMNode::getUInt(const char* name) {
	string str = getStr(name);
	int result = 0;
	sscanf(str.c_str(),"%d",&result);
	return result;
}

float FIOMNode::getFloat(const char* name) {
	string str = getStr(name);
	float result = 0;
	sscanf(str.c_str(),"%f",&result);
	return result;
}

bool FIOMNode::getBool(const char* name) {
	string str = getStr(name);
	if( "true" == str || "1" == str || "YES" == str || "yes" == str) {
		return true;
	}
	return false;
}

cocos2d::CCPoint FIOMNode::getPoint(const char* name) {
	FIOMNode node = this->operator[](name);
	return ccp(node.getFloat("x"),node.getFloat("y"));
}

CCSize FIOMNode::getSize(const char* name) {
	FIOMNode node = this->operator[](name);
	return CCSize(node.getFloat("width"),node.getFloat("height"));
}

CCRect FIOMNode::getRect(const char* name) {
	FIOMNode node = this->operator[](name);
	return CCRect(node.getFloat("x"), node.getFloat("y"), node.getFloat("width"), node.getFloat("height"));
}

ccColor3B FIOMNode::getColor(const char* name) {
	FIOMNode node = this->operator[](name);
	return ccc3(node.getFloat("r"),node.getFloat("g"),node.getFloat("b"));
}

#define SET_INIT(TYPE) \
void FIOMNode::set(std::string string,TYPE value) { \
this->set(string.c_str(),value); \
} \
void FIOMNode::set(unsigned int number,TYPE value) { \
	this->set(itoa(number),value); \
} \
void FIOMNode::set(int number,TYPE value) { \
	this->set(itoa(number),value); \
}

#define GET_INIT(TYPE,TYPE_PREFIX) \
TYPE FIOMNode::get##TYPE_PREFIX(std::string string) { \
return this->get##TYPE_PREFIX(string.c_str()); \
} \
TYPE FIOMNode::get##TYPE_PREFIX(unsigned int number) { \
return this->get##TYPE_PREFIX(itoa(number)); \
} \
TYPE FIOMNode::get##TYPE_PREFIX(int number) { \
	return this->get##TYPE_PREFIX(itoa(number)); \
}

SET_INIT(int)
SET_INIT(const char*)
SET_INIT(unsigned int)
SET_INIT(unsigned long long int)
SET_INIT(std::string)
SET_INIT(float)
SET_INIT(bool)
SET_INIT(cocos2d::CCPoint)
SET_INIT(cocos2d::CCSize)
SET_INIT(cocos2d::CCRect)
SET_INIT(cocos2d::ccColor3B)


GET_INIT(int,Int)
GET_INIT(unsigned int,UInt)
GET_INIT(unsigned long long int,ULInt)
GET_INIT(std::string,Str)
GET_INIT(float,Float)
GET_INIT(bool,Bool)
GET_INIT(cocos2d::CCPoint,Point)
GET_INIT(cocos2d::CCSize,Size)
GET_INIT(cocos2d::CCRect,Rect)
GET_INIT(cocos2d::ccColor3B,Color)
