#ifndef _GRIZZLY_JR_FILE_IMAGE_ON_MEMORY_MORE_FUNCTION_H_
#define _GRIZZLY_JR_FILE_IMAGE_ON_MEMORY_MORE_FUNCTION_H_

#include "FIOMMain.h"
#include "cocos2d.h"
#include <GrizzlyJr/Math/Point2D.h>
#include <string>

namespace GrizzlyJr
{
	//#define FIOM FIOMMain
	
	#define FIOM_NODE FIOMMain::NodeFileImageResult

	#pragma warning(disable:4996)
	class FileImageOnMemoryMoreFunction
	{
	public:
		static inline FIOM_NODE getInfo(FilePath path) {
			return FIOMMain::get()->getInfo(path);
		}
		static inline FIOM_NODE next(FIOM_NODE& node,const char* name) {
			if( 0x0 == name) {
				return node;
			}
			return FIOMMain::get()->getInfo(node, name);
		}
		static inline FIOM_NODE next_set(FIOM_NODE& node,const char* name) {
			if( 0x0 == name) {
				return node;
			}
			
			FIOM_NODE find = FIOMMain::get()->getInfo(node, name);
			if( 0x0 == find.node) {
				FIOMMain::NodeFileImage newNode;
				newNode.isArray = false;
				newNode.isSpecial = false;
				newNode.dirs.clear();
				newNode.values.clear();
				
				node.node->dirs[name] = newNode;
				return FIOMMain::get()->getInfo(node, name);
			}
			return find;
		}
		static inline void clear(FIOM_NODE& node) {
			if( 0x0 == node.node) {
				return;
			}
			node.node->dirs.clear();
			node.node->values.clear();
			node.node->isArray = false;
			node.node->isSpecial = false;
		}
		
		static inline FIOM_NODE next(FIOM_NODE& node,unsigned int number) {
			char per[100] = {0};
			sprintf(per, "%d",number);
			return FIOMMain::get()->getInfo(node, per);
		}
		
		static inline void setstr(FIOM_NODE& node, const char* name,std::string value) {
			FIOMMain::get()->setStr(node,name,value);
		}	
	
		static inline void setint(FIOM_NODE& node, const char* name, int value) {
			char per[100] = {0};
			sprintf(per, "%d",value);
			return setstr(node, name, per);
		}
		
		static inline void setfloat(FIOM_NODE& node, const char* name, float value) {
			char per[100] = {0};
			sprintf(per, "%f",value);
			return setstr(node, name, per);
		}
		
		static inline void setbool(FIOM_NODE& node, const char* name, bool value) {
			if( true == value) {
				return setstr(node, name, "true");
			}
			return setstr(node, name, "false");
		}
		
		static inline void setCCPoint(FIOM_NODE node, const char* name, cocos2d::CCPoint point) {
			node = next_set(node,name);
			setfloat(node,"x", point.x); 
			setfloat(node,"y", point.y);
		}
		static inline void setCCSize(FIOM_NODE node, const char* name, cocos2d::CCSize size) {
			node = next_set(node,name);
			setfloat(node,"width", size.width); 
			setfloat(node,"height", size.height);
		}
		static inline void setCCRect(FIOM_NODE& node, const char* name, cocos2d::CCRect rect) {
			setCCPoint(node, name,rect.origin); 
			setCCSize(node, name,rect.size);
		}

		static inline void setPoint2Di(FIOM_NODE node, const char* name, Point2Di point) {
			node = next_set(node,name);
			setint(node,"x", point.x); 
			setint(node,"y", point.y);
		}
		static inline void setPoint2Df(FIOM_NODE node, const char* name, Point2Df point) {
			node = next_set(node,name);
			setfloat(node,"x", point.x); 
			setfloat(node,"y", point.y);
		}
		
		static inline void setccColor3B(FIOM_NODE node, const char* name, cocos2d::ccColor3B color) {
			node = next_set(node,name);
			setint(node,"r", (int)color.r); 
			setint(node,"g", (int)color.g);
			setint(node,"b", (int)color.b);
		}
	
		static inline std::string getstr(FIOM_NODE& node, const char* name) {
			return FIOMMain::get()->getStr(node, name);
		}
		
		static inline int getint(FIOM_NODE& node, const char* name) {
			std::string str = FIOMMain::get()->getStr(node, name);
			int result = 0;
			sscanf(str.c_str(),"%d",&result);
			return result;
		}
		static inline float getfloat(FIOM_NODE& node, const char* name) {
			std::string str = FIOMMain::get()->getStr(node, name);
			float result = 0;
			sscanf(str.c_str(),"%f",&result);
			return result;
		}
		
		static inline bool getbool(FIOM_NODE& node, const char* name) {
			std::string str = FIOMMain::get()->getStr(node, name);
			if( "true" == str || "1" == str || "YES" == str || "yes" == str) {
				return true;
			}
			return false;
		}
		
		static inline cocos2d::CCPoint getCCPoint(FIOM_NODE node, const char* name) {
			node = next(node,name);
			return ccp(getfloat(node,"x"), getfloat(node,"y"));
		}
		static inline cocos2d::CCSize getCCSize(FIOM_NODE node, const char* name) {
			node = next(node,name);
			return cocos2d::CCSize(getfloat(node,"width"), getfloat(node,"height"));
		}
		static inline cocos2d::CCRect getCCRect(FIOM_NODE node, const char* name) {
			node = next(node,name);
			return cocos2d::CCRectMake(	getfloat(node,"x"), getfloat(node,"y"),
										getfloat(node,"width"), getfloat(node,"height"));
		}
		
		static inline Point2Di getPoint2Di(FIOM_NODE node, const char* name) {
			node = next(node,name);
			return Point2Di(getint(node,"x"), getint(node,"y"));
		}
		static inline Point2Df getPoint2Df(FIOM_NODE node, const char* name) {
			node = next(node,name);
			return Point2Df(getfloat(node,"x"), getfloat(node,"y"));
		}
		
		static inline cocos2d::ccColor3B getccColor3B(FIOM_NODE node, const char* name) {
			node = next(node,name);
			return cocos2d::ccc3((GLubyte)getint(node,"r"), (GLubyte)getint(node,"g"),(GLubyte)getint(node,"b"));
		}
		
		
		
		static inline std::string getLstr(FIOM_NODE node, const char* name) {
			return getstr(node,name);
		}
		
		static inline int getLint(FIOM_NODE node, const char* name) {
			return getint(node,name);
		}
		static inline float getLfloat(FIOM_NODE node, const char* name) {
			return getfloat(node,name);
		}
		
		static inline bool getLbool(FIOM_NODE node, const char* name) {
			return getbool(node,name);
		}
		
		static inline cocos2d::CCPoint getLCCPoint(FIOM_NODE node, const char* name) {
			return getCCPoint(node,name);
		}
		static inline cocos2d::CCSize getLCCSize(FIOM_NODE node, const char* name) {
			return getCCSize(node,name);
		}
		static inline cocos2d::CCRect getLCCRect(FIOM_NODE node, const char* name) {
			return getCCRect(node,name);
		}
		
		static inline Point2Di getLPoint2Di(FIOM_NODE node, const char* name) {
			return getPoint2Di(node,name);
		}
		static inline Point2Df getLPoint2Df(FIOM_NODE node, const char* name) {
			return getPoint2Df(node,name);
		}
		
		static inline cocos2d::ccColor3B getLccColor3B(FIOM_NODE node, const char* name) {
			return getccColor3B(node,name);
		}
		
		static std::vector<std::string> getArrayValues(FIOM_NODE node) {
			std::vector<std::string> result;
			if( 0x0 == node.node || !node.node->isArray) {
				return result;
			}
			unsigned int size = node.node->values.size();
			result.resize(size);
			
			size += node.node->dirs.size();
			unsigned int k = 0;
			for( unsigned int i = 0; i < size; i++) {
				char data[10] = {0};
				sprintf(data,"%d",i);
				if( node.node->values.end() != node.node->values.find(data)) {
					result[k] = node.node->values[data];
					k++;
				}
			}
			return result;
		}
		
		static std::vector<FIOM_NODE> getArrayCatalog(FIOM_NODE node) {
			std::vector<FIOM_NODE> result;
			if( 0x0 == node.node || !node.node->isArray) {
				return result;
			}
			
			unsigned int size = node.node->dirs.size();
			result.resize(size);
			
			size += node.node->values.size();
			unsigned int k = 0;
			for( unsigned int i = 0; i < size; i++) {
				char data[10] = {0};
				sprintf(data,"%d",i);
				if( node.node->dirs.end() != node.node->dirs.find(data)) {
					FIOM_NODE newNode;
					newNode.node = &node.node->dirs[data];
					newNode.path = node.path; newNode.path.addPath(data);
					result[k] = newNode;
					k++;
				}
			}
			return result;
		}
		
		static std::vector<std::string> getCatalogNames(FIOM_NODE node) {
			std::vector<std::string> result;
			if( 0x0 == node.node) {
				return result;
			}
			
			result.resize(node.node->dirs.size());
			std::map<std::string,FIOMMain::NodeFileImage>::iterator iter;
			unsigned int i =0;
			for( iter = node.node->dirs.begin(); iter != node.node->dirs.end();iter++) {
				result[i] = iter->first;
				i++;
			}
			
			return result;
		}
		
		static std::vector<std::string> getValueNames(FIOM_NODE node) {
			std::vector<std::string> result;
			if( 0x0 == node.node) {
				return result;
			}
			
			result.resize(node.node->values.size());
			std::map<std::string,std::string>::iterator iter;
			unsigned int i =0;
			for( iter = node.node->values.begin(); iter != node.node->values.end();iter++) {
				result[i] = iter->first;
				i++;
			}
			
			return result;
		}
		
		static inline unsigned int getCountValues(FIOM_NODE node) {
			if( 0x0 == node.node) {
				return 0;
			}
			return node.node->values.size();
		}
		static inline unsigned int getCountCatalogs(FIOM_NODE node) {
			if( 0x0 == node.node) {
				return 0;
			}
			return node.node->dirs.size();
		}
		
		static std::string getStrFromInt(int value) {
			char data[16] = {0};
			sprintf(data,"%d",value);
			return data;
		}
	};
	
	#define FIOMM FileImageOnMemoryMoreFunction
	#define FIOMG(type,node,name) FileImageOnMemoryMoreFunction::get##type(node,name)
	#define FIOMGL(type,node,name) FileImageOnMemoryMoreFunction::getL##type(node,name)
	
	#define FIOMP(pathInfo) FileImageOnMemoryMoreFunction::getInfo(pathInfo)

	#pragma warning(default:4996)
};
#endif
