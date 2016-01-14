/****************************************************************************
Copyright (c) 2012-2013 grizzly-jr
Copyright (c) 2012-2013 Ivlev Alexander 
****************************************************************************/

#ifndef _COMPANY_GRIZZLY_JR__PACKAGE_FIOM__CLASS_NODE_H_
#define _COMPANY_GRIZZLY_JR__PACKAGE_FIOM__CLASS_NODE_H_

#include "../Defines.h"

#ifdef GRIZZLY_JR_FION_ENABLE_COCOS2D
#include "cocos2d.h"
#endif

#include "GJFIOMValue.h"
#include "GJFIOMHashMap.h"
#include <string>
#include <vector>

namespace GrizzlyJr {
	namespace FIOM {
		class GJFIOMNode {
		public:
			enum GJFIOMNodeType {
				NODE_NOT_INIT,
				NODE_DICT,
				NODE_ARRAY,
				NODE_SPECIAL
			};
		protected:
			///TODO: создать hash_map свою реализацию - обращение к массиву не будет задерживать

			struct GJFIOMInfoNode {
				GJFIOMHashMap<GJFIOMNode> childs;
				GJFIOMHashMap<GJFIOMValue> values;
				GJFIOMNodeType type;
				GJFIOMInfoNode() { type = NODE_NOT_INIT;}
			};
			GJFIOMInfoNode* data;
			size_t* retainCount;

			static char* itoa(unsigned int i);
		public:
			GJFIOMNode();
			GJFIOMNode(const GJFIOMNode& copy);
			~GJFIOMNode();
			inline bool isCorrect()const { return NODE_NOT_INIT!=data->type; }

			size_t size()const;

			std::vector<std::string> allChildsKeys()const;
			std::vector<std::string> allValuesKeys()const;

			const GJFIOMNode& operator[](std::string name)const;
			const GJFIOMNode& operator[](const char* name)const;
			const GJFIOMNode& operator[](size_t index)const;
			const GJFIOMValue& operator()(std::string name)const;
			const GJFIOMValue& operator()(const char* name)const;
			const GJFIOMValue& operator()(size_t index)const;

			bool isChild(std::string name)const;
			bool isChild(const char* name)const;
			bool isChild(size_t index)const;

			bool isValue(std::string name)const;
			bool isValue(const char* name)const;
			bool isValue(size_t index)const;

			void setType(GJFIOMNodeType type)const;
			inline GJFIOMNodeType getType()const { return data->type;}

			inline bool isDict()const { return NODE_DICT ==data->type;}
			inline bool isArray()const { return NODE_ARRAY ==data->type;}
			inline bool isSpecial()const { return NODE_SPECIAL ==data->type;}

			bool isEqual(const GJFIOMNode& node)const;

#ifdef GRIZZLY_JR_FION_ENABLE_COCOS2D
			operator cocos2d::CCPoint()const;
			operator cocos2d::CCSize()const;
			operator cocos2d::CCRect()const;
			operator cocos2d::ccColor3B()const;
			operator cocos2d::ccColor4B()const;
			operator cocos2d::ccColor4F()const;
#endif

			const GJFIOMNode& operator=(const GJFIOMNode& value)const;
#ifdef GRIZZLY_JR_FION_ENABLE_COCOS2D
			const GJFIOMNode& operator=(const cocos2d::CCPoint value)const;
			const GJFIOMNode& operator=(const cocos2d::CCSize value)const;
			const GJFIOMNode& operator=(const cocos2d::CCRect value)const;
			const GJFIOMNode& operator=(const cocos2d::ccColor3B value)const;
			const GJFIOMNode& operator=(const cocos2d::ccColor4B value)const;
			const GJFIOMNode& operator=(const cocos2d::ccColor4F value)const;
#endif

		protected:
			virtual size_t sizeSpecial()const;
			virtual std::vector<std::string> allChildsKeysSpecial()const;
			virtual std::vector<std::string> allValuesKeysSpecial()const;

			virtual const GJFIOMNode& getChild(std::string name)const;
			virtual const GJFIOMValue& getValue(std::string name)const;

			virtual bool isChildSpecial(std::string name)const;
			virtual bool isValueSpecial(std::string name)const;
		};
	};
};

#endif