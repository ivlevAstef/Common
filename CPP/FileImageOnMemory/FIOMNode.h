//
//  FIOMNode.h
//  CuteHeroes
//
//  Created by Alexander Ivlev on 06.08.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef _GRIZZLY_JR_FILE_IMAGE_ON_MEMORY_NODE_H_
#define _GRIZZLY_JR_FILE_IMAGE_ON_MEMORY_NODE_H_

#include "FIOMMain.h"
#include "cocos2d.h"

namespace GrizzlyJr
{	
	class FIOMNode
	{
	protected:
		FIOMMain::NodeFileImage* node;
	public:
		static FIOMNode get(const char* name);
		
		FIOMNode():node(0x0) {}
		FIOMNode(FIOMMain::NodeFileImage* node);
		FIOMNode(const FIOMNode& node);
		const FIOMNode& operator=(const FIOMNode& copy);
		
		inline bool isCorrect()const { return node != 0x0; }
		inline bool isArray()const { return node->isArray; }
		inline bool isSpecial()const { return node->isSpecial; }
		
		size_t getArraySize();
		void resize(size_t newSize);
		std::vector<std::string> getValueNames();
		std::vector<std::string> getCatalogNames();
		
		FIOMNode addNextDepth(unsigned int number,bool isArray,bool isSpecial)const;
		FIOMNode addNextDepth(std::string str,bool isArray,bool isSpecial)const;
		FIOMNode addNextDepth(const char* name,bool isArray,bool isSpecial)const;
		FIOMNode clear();
		
		FIOMNode addNextDepth(unsigned int number,FIOMNode node)const;
		FIOMNode addNextDepth(std::string str,FIOMNode node)const;
		FIOMNode addNextDepth(const char* name,FIOMNode node)const;
		
		FIOMNode set(unsigned int number,FIOMNode node)const;
		FIOMNode set(std::string str,FIOMNode node)const;
		FIOMNode set(const char* name,FIOMNode node)const;
		
		///ADD IF DON'T FIND
		FIOMNode operator[](const char* name)const;
		FIOMNode operator[](std::string name)const;
		FIOMNode operator[](unsigned int number)const;
		FIOMNode operator[](int number)const { return operator[]((unsigned int)number); }
		
		bool is(std::string name);
		bool is(const char* name);
		bool is(unsigned int number);
		bool is(int number) { return is((unsigned int)number); }
		
#define SET_CREATE(TYPE) \
		void set(std::string name,TYPE value); \
		void set(const char* name,TYPE value); \
		void set(unsigned int number,TYPE value); \
		void set(int number,TYPE value);
		
		SET_CREATE(std::string)
		SET_CREATE(const char*)
		SET_CREATE(int)
		SET_CREATE(unsigned int)
		SET_CREATE(unsigned long long int)
		SET_CREATE(float)
		SET_CREATE(bool)
		SET_CREATE(cocos2d::CCPoint)
		SET_CREATE(cocos2d::CCSize)
		SET_CREATE(cocos2d::CCRect)
		SET_CREATE(cocos2d::ccColor3B)
		
#undef SET_CREATE		

#define GET_CREATE(NAME_TYPE,TYPE) \
TYPE get##NAME_TYPE(std::string name); \
TYPE get##NAME_TYPE(const char* name); \
TYPE get##NAME_TYPE(unsigned int number); \
TYPE get##NAME_TYPE(int number);
	
		GET_CREATE(Str,std::string)
		GET_CREATE(Int,int)
		GET_CREATE(UInt,unsigned int)
		GET_CREATE(ULInt,unsigned long long int)
		GET_CREATE(Float,float)
		GET_CREATE(Bool,bool)
		GET_CREATE(Point,cocos2d::CCPoint)
		GET_CREATE(Size,cocos2d::CCSize)
		GET_CREATE(Rect,cocos2d::CCRect)
		GET_CREATE(Color,cocos2d::ccColor3B)
		
#undef GET_CREATE
	};
};
#endif

