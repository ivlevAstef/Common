/****************************************************************************
Copyright (c) 2012-2013 grizzly-jr
Copyright (c) 2012-2013 Ivlev Alexander 
****************************************************************************/

#include "GJFIOMNode.h"

using namespace GrizzlyJr;
using namespace FIOM;

#ifdef GRIZZLY_JR_FION_ENABLE_COCOS2D
USING_NS_CC;
#endif

char* GJFIOMNode::itoa(unsigned int value) {
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

GJFIOMNode::GJFIOMNode() {
	data = new GJFIOMInfoNode();
	retainCount = new size_t();
	*retainCount = 1;
}
GJFIOMNode::GJFIOMNode(const GJFIOMNode& copy) {
	if( this == &copy) {
		return;
	}

	this->data = copy.data;
	this->retainCount = copy.retainCount;
	(*this->retainCount)++;
}
GJFIOMNode::~GJFIOMNode() {
	(*retainCount)--;
	if( 0 == (*retainCount) ) {
		delete data;
		delete retainCount;
	}
}

size_t GJFIOMNode::size()const {
	if( NODE_ARRAY == data->type) {
		return data->childs.size()+data->values.size();
	}
	if( NODE_SPECIAL == data->type) {
		return sizeSpecial();
	}

	return 0;
}

std::vector<std::string> GJFIOMNode::allChildsKeys()const {
	if( NODE_SPECIAL == data->type) {
		return allChildsKeysSpecial();
	}

	if( NODE_DICT == data->type) {
		return data->childs.getAllKeys();
	}
	return std::vector<std::string>();
}
std::vector<std::string> GJFIOMNode::allValuesKeys()const {
	if( NODE_SPECIAL == data->type) {
		return allValuesKeysSpecial();
	}

	if( NODE_DICT == data->type) {
		return data->values.getAllKeys();
	}
	return std::vector<std::string>();
}

const GJFIOMNode& GJFIOMNode::getChild(std::string name)const {
	return this->data->childs.get(name);
}
const GJFIOMValue& GJFIOMNode::getValue(std::string name)const {
	return this->data->values.get(name);
}

const GJFIOMNode& GJFIOMNode::operator[](std::string name)const {
	return this->getChild(name);
}
const GJFIOMNode& GJFIOMNode::operator[](const char* name)const {
	return this->getChild(std::string(name));
}
const GJFIOMNode& GJFIOMNode::operator[](size_t index)const {
	return this->getChild(std::string(itoa(index)));
}

const GJFIOMValue& GJFIOMNode::operator()(std::string name)const {
	return this->getValue(name);
}
const GJFIOMValue& GJFIOMNode::operator()(const char* name)const {
	return this->getValue(std::string(name));
}
const GJFIOMValue& GJFIOMNode::operator()(size_t index)const {
	return this->getValue(std::string(itoa(index)));
}

bool GJFIOMNode::isChild(std::string name)const {
	if( NODE_SPECIAL == this->data->type) {
		return isChildSpecial(name);
	}
	return this->data->childs.is(name);
}
bool GJFIOMNode::isChild(const char* name)const {
	return isChild(std::string(name));
}
bool GJFIOMNode::isChild(size_t index)const {
	return isChild(std::string(itoa(index)));
}

bool GJFIOMNode::isValue(std::string name)const {
	if( NODE_SPECIAL == this->data->type) {
		return isValueSpecial(name);
	}
	return this->data->values.is(name);
}
bool GJFIOMNode::isValue(const char* name)const {
	return isValue(std::string(name));
}
bool GJFIOMNode::isValue(size_t index)const {
	return isValue(std::string(itoa(index)));
}

bool GJFIOMNode::isEqual(const GJFIOMNode& node)const {
	if( this->data == node.data) {
		return true;
	}
	return false;
}

void GJFIOMNode::setType(GJFIOMNodeType type)const {
	this->data->type = type;
}

#ifdef GRIZZLY_JR_FION_ENABLE_COCOS2D
GJFIOMNode::operator cocos2d::CCPoint()const {
	return CCPoint(	(float)(*this)("x"),(float)(*this)("y"));
}
GJFIOMNode::operator cocos2d::CCSize()const {
	return CCSize(	(float)(*this)("width"),(float)(*this)("height"));
}
GJFIOMNode::operator cocos2d::CCRect()const {
	return CCRect(	(float)(*this)("x"),(float)(*this)("y"),
					(float)(*this)("width"),(float)(*this)("height"));
}
GJFIOMNode::operator cocos2d::ccColor3B()const {
	return ccc3((int)(*this)("r"),(int)(*this)("g"),(int)(*this)("b"));
}
GJFIOMNode::operator cocos2d::ccColor4B()const {
	return ccc4((int)(*this)("r"),(int)(*this)("g"),(int)(*this)("b"),(int)(*this)("a"));
}
GJFIOMNode::operator cocos2d::ccColor4F()const {
	return ccc4f((float)(*this)("r"),(float)(*this)("g"),(float)(*this)("b"),(float)(*this)("a"));
}
#endif

const GJFIOMNode& GJFIOMNode::operator=(const GJFIOMNode& value)const {
	if( this == &value) {
		return *this;
	}
	this->data->childs.copy(value.data->childs);
	this->data->values.copy(value.data->values);
	this->data->type = value.data->type;
	return *this;
}

#ifdef GRIZZLY_JR_FION_ENABLE_COCOS2D
const GJFIOMNode& GJFIOMNode::operator=(const cocos2d::CCPoint value)const {
	this->data->values["x"] = (float)value.x;
	this->data->values["y"] = (float)value.y;
	return *this;
}
const GJFIOMNode& GJFIOMNode::operator=(const cocos2d::CCSize value)const {
	this->data->values["width"] = (float)value.width;
	this->data->values["height"] = (float)value.height;
	return *this;
}
const GJFIOMNode& GJFIOMNode::operator=(const cocos2d::CCRect value)const {
	this->data->values["x"] = (float)value.origin.x;
	this->data->values["y"] = (float)value.origin.y;
	this->data->values["width"] = (float)value.size.width;
	this->data->values["height"] = (float)value.size.height;
	return *this;
}
const GJFIOMNode& GJFIOMNode::operator=(const cocos2d::ccColor3B value)const {
	this->data->values["r"] = (int)value.r;
	this->data->values["g"] = (int)value.g;
	this->data->values["b"] = (int)value.b;
	return *this;
}
const GJFIOMNode& GJFIOMNode::operator=(const cocos2d::ccColor4B value)const {
	this->data->values["r"] = (int)value.r;
	this->data->values["g"] = (int)value.g;
	this->data->values["b"] = (int)value.b;
	this->data->values["a"] = (int)value.a;
	return *this;
}
const GJFIOMNode& GJFIOMNode::operator=(const cocos2d::ccColor4F value)const {
	this->data->values["r"] = (float)value.r;
	this->data->values["g"] = (float)value.g;
	this->data->values["b"] = (float)value.b;
	this->data->values["a"] = (float)value.a;
	return *this;
}
#endif

size_t GJFIOMNode::sizeSpecial()const {
	return 0;
}
std::vector<std::string> GJFIOMNode::allChildsKeysSpecial()const {
	return data->childs.getAllKeys();
}
std::vector<std::string> GJFIOMNode::allValuesKeysSpecial()const {
	return data->values.getAllKeys();
}

bool GJFIOMNode::isChildSpecial(std::string name)const {
	return this->data->childs.is(name);
}
bool GJFIOMNode::isValueSpecial(std::string name)const {
	return this->data->values.is(name);
}