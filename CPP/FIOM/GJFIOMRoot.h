/****************************************************************************
Copyright (c) 2012-2013 grizzly-jr
Copyright (c) 2012-2013 Ivlev Alexander 
****************************************************************************/

#ifndef _COMPANY_GRIZZLY_JR__PACKAGE_FIOM__CLASS_ROOT_H_
#define _COMPANY_GRIZZLY_JR__PACKAGE_FIOM__CLASS_ROOT_H_

#include "GJFIOMNode.h"

namespace GrizzlyJr {
	namespace FIOM {
		class GJFIOMRoot: public GJFIOMNode {
		public:
			enum GJFIOMPathType {
				GJFIOM_CHANGEABLE_CATALOG,
				GJFIOM_READ_ONLY_CATALOG,
				GJFIOM_NET_CATALOG,
				GJFIOM_USER_CATALOG,
				GJFIOM_COUNT_PATH_TYPE
			};
			typedef unsigned char* (*FunctionForLoadData)(std::string path,unsigned long* sizeData);
			typedef bool (*FunctionForSaveData)(std::string path,const unsigned char* data,unsigned long sizeData);

			typedef std::string (*FunctionGetWriteablePath)(std::string relativePath);
			typedef std::string (*FunctionGetReadOnlyPath)(std::string relativePath);
		private:
			static GJFIOMRoot singleton;

			struct GJFIOMEasyString {
				const char* word;
				size_t size;
			};
			struct GJFIOMNodeWithWords {
				GJFIOMNode node;
				GJFIOMEasyString* words;
				size_t count;

				GJFIOMNodeWithWords(GJFIOMNode& node,GJFIOMEasyString* words,size_t count) {
					this->node = node;
					this->words = words;
					this->count = count;
				}
			};
			FunctionForLoadData userFunctionLoad;
			FunctionForSaveData userFunctionSave;

			FunctionGetWriteablePath getWriteablePath;
			FunctionGetReadOnlyPath getReadOnlyPath;
		public:
			static const GJFIOMRoot& get();

			static void changeWriteablePathFunction(FunctionGetWriteablePath newFunction);
			static void changeReadOnlyPathFunction(FunctionGetReadOnlyPath newFunction);

			///default load priority: changeable, readonly, net, user
			static bool load(std::string path);
			static bool load(std::string path,size_t count,GJFIOMPathType typeFirst,...);
			///load use memory 
			static bool load(const char* name,const unsigned char* data,unsigned long size);

			///default save: changeable
			static bool save(std::string pathOrName);
			static bool save(std::string pathOrName,GJFIOMPathType type);
			static bool save(std::string pathOrName,const GJFIOMNode& node,GJFIOMPathType type);

			static bool close(std::string name);

			///Utils function
			static std::string removeAllMultiSlash(const char* data);
			static std::string getNameForPath(std::string name);
		private:
			GJFIOMRoot();
			~GJFIOMRoot();

			///Standard Load function data
			static unsigned char* getDataFromPathChangeable(std::string path,unsigned long* sizeData);
			static unsigned char* getDataFromPathReadOnly(std::string path,unsigned long* sizeData);
			static unsigned char* getDataFromPathNET(std::string path,unsigned long* sizeData);

			///Standard Save function data
			static bool saveDataByPathChangeable(std::string path,const unsigned char* data,unsigned long sizeData);
			static bool saveDataByPathNET(std::string path,const unsigned char* data,unsigned long sizeData);

			///function convert data in tree inside memory
			static bool loadUnSaveData(const char* name,unsigned char* data,unsigned long size);
			///Parsing function on load
			static GJFIOMEasyString* parsingOnWord(unsigned char* literals, unsigned long size, size_t& count);
			static GJFIOMNodeWithWords createFIOMUseWords(GJFIOMEasyString* words,size_t count);

			static std::string convertMemoryStringOnFileString(std::string);
			static std::string getTabulation(int tabulation);
			static bool convertNodeOnData(const GJFIOMNode& node,std::string& stream, int tabulation);
		};
	};
};

#endif