/****************************************************************************
Copyright (c) 2012-2013 grizzly-jr
Copyright (c) 2012-2013 Ivlev Alexander 
****************************************************************************/

#include "GJFIOMValue.h"

using namespace GrizzlyJr;
using namespace FIOM;

GJFIOMValue::GJFIOMValueInformation::GJFIOMValueInformation():str(0x0),countSymbols(0) {

}
GJFIOMValue::GJFIOMValueInformation::~GJFIOMValueInformation() {
	if( 0x0 != str) {
		free(str);
	}
	countSymbols = 0;
}

GJFIOMValue::GJFIOMValueInformation::GJFIOMValueInformation(const GJFIOMValueInformation& copy) {
	if( &copy == this) {
		return;
	}

	setString(copy.str);
}

void GJFIOMValue::GJFIOMValueInformation::copy(const GJFIOMValueInformation& copy) {
	if( &copy == this) {
		return;
	}

	setString(copy.str);
}


void GJFIOMValue::GJFIOMValueInformation::setString(const char* newstr) {
	if( 0x0 != str) {
		free(str);
		str = 0x0;
	}
	countSymbols = 0;

	if( 0x0 == newstr) {
		return;
	}

	const char* iter = newstr;
	while( ++countSymbols,'\0' != *(iter++));

	str = (char*)malloc(countSymbols*sizeof(char));
	memcpy(str,newstr,countSymbols*sizeof(char));
}

bool GJFIOMValue::GJFIOMValueInformation::isEqual(const char* str2) {
	if( 0x0 == str || 0x0 == str2) {
		return false;
	}
	const char* iter = str;
	while(*iter == *str2) {
		if( '\0' == *iter) {
			return true;
		}
		++iter;
		++str2;
	}
	return false;
}

GJFIOMValue::GJFIOMValue() {
	value = new GJFIOMValueInformation();
	retainCount = new size_t();
	*retainCount = 1;
}
GJFIOMValue::GJFIOMValue(const GJFIOMValue& copy) {
	if( this == &copy) {
		return;
	}
	
	this->value = copy.value;
	this->retainCount = copy.retainCount;
	(*this->retainCount)++;
}
GJFIOMValue::~GJFIOMValue() {
	(*retainCount)--;
	if( 0 == (*retainCount) ) {
		delete value;
		delete retainCount;
	}
}

GJFIOMValue::operator std::string()const {
	return value->str;
}
GJFIOMValue::operator const char*()const {
	return value->str;
}
GJFIOMValue::operator char()const {
	int result = 0;
	if( 0x0 != value->str && 1 != sscanf(value->str, "%d",&result)) {
		return 0;
	}
	return (char)result;
}

GJFIOMValue::operator short()const {
	int result = 0;
	if( 0x0 != value->str && 1 != sscanf(value->str, "%d",&result)) {
		return 0;
	}
	return (short)result;
}
GJFIOMValue::operator int()const {
	int result = 0;
	if( 0x0 != value->str && 1 != sscanf(value->str, "%d",&result)) {
		return 0;
	}
	return (int)result;
}

GJFIOMValue::operator long long int()const {
	long long int result = 0;
	if( 0x0 != value->str && 1 != sscanf(value->str, "%lld",&result)) {
		return 0;
	}
	return (long long int)result;
}

GJFIOMValue::operator unsigned int()const {
	unsigned int result = 0;
	if( 0x0 != value->str && 1 != sscanf(value->str, "%u",&result)) {
		return 0;
	}
	return (unsigned int)result;
}
GJFIOMValue::operator float()const {
	float result = 0;
	if( 0x0 != value->str && 1 != sscanf(value->str, "%f",&result)) {
		return 0;
	}
	return (float)result;
}
GJFIOMValue::operator double()const {
	double result = 0;
	if( 0x0 != value->str && 1 != sscanf(value->str, "%lf",&result)) {
		return 0;
	}
	return (double)result;
}
GJFIOMValue::operator bool()const {
	return (bool)(value->isEqual("true") || value->isEqual("1"));
}

const GJFIOMValue& GJFIOMValue::operator=(const GJFIOMValue& copy)const {
	if( this == &copy) {
		return *this;
	}
	this->value->copy(*copy.value);
	return *this;
}
const GJFIOMValue& GJFIOMValue::operator=(const std::string value)const {
	this->value->setString(value.c_str());
	return *this;
}
const GJFIOMValue& GJFIOMValue::operator=(const char* value)const {
	this->value->setString(value);
	return *this;
}
const GJFIOMValue& GJFIOMValue::operator=(const short value)const {
	char temp[16] = {0};
	sprintf(temp, "%hd",value);
	this->value->setString(temp);
	return *this;
}
const GJFIOMValue& GJFIOMValue::operator=(const int value)const {
	char temp[16] = {0};
	sprintf(temp, "%d",value);
	this->value->setString(temp);
	return *this;
}
const GJFIOMValue& GJFIOMValue::operator=(const long long int value)const {
	char temp[32] = {0};
	sprintf(temp, "%lld",value);
	this->value->setString(temp);
	return *this;
}
const GJFIOMValue& GJFIOMValue::operator=(const unsigned int value)const {
	char temp[16] = {0};
	sprintf(temp, "%u",value);
	this->value->setString(temp);
	return *this;
}
const GJFIOMValue& GJFIOMValue::operator=(const float value)const {
	char temp[32] = {0};
	sprintf(temp, "%f",value);
	this->value->setString(temp);
	return *this;
}
const GJFIOMValue& GJFIOMValue::operator=(const double value)const {
	char temp[32] = {0};
	sprintf(temp, "%lf",value);
	this->value->setString(temp);
	return *this;
}
const GJFIOMValue& GJFIOMValue::operator=(const bool value)const {
	if( value) {
		this->value->setString("true");
	} else {
		this->value->setString("false");
	}
	
	return *this;
}