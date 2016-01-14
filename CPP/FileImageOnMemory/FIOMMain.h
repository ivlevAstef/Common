#ifndef _GRIZZLY_JR_FILE_IMAGE_ON_MEMORY_H_
#define _GRIZZLY_JR_FILE_IMAGE_ON_MEMORY_H_

#include "FilePath.h"
#include <map>
#include <string>
#include <vector>
#include "../../../../CreptingFiles/EncryptDecrypt.h"

namespace GrizzlyJr
{	
	class FIOMNode;
	class FIOMMain
	{
	public:
		struct NodeFileImage {
			std::map<std::string,std::string> values;
			std::map<std::string,NodeFileImage> dirs;
			bool isSpecial;
			bool isArray;
			
			NodeFileImage():isSpecial(false),isArray(false) {}
			
			~NodeFileImage() {
				values.clear();
				dirs.clear();
			}
		};
		
		struct NodeFileImageResult {
			NodeFileImage* node;
			FilePath path;
		};
	private:
		std::map<std::string,NodeFileImage> files;

		static FIOMMain* singleton;
		EncryptDecrypt crepting;
		
	public:
		FIOMMain();

		std::string loadFile(std::string path);///return NameFile
		bool loadFileData(std::string name,unsigned char* dataFile, unsigned long sizeFile);
		void saveFile(std::string path);
		void closeFile(std::string path);
		bool isLoad(std::string path);
		std::string getNameFromPath(std::string path);
		
		FIOMNode getRoot(const char* nameFile);
		FIOMNode getNodeByPath(FilePath path);

		static FIOMMain* get();
		
		bool isPath(FilePath path);///Deprecated Method
		NodeFileImageResult getInfo(FilePath path);///Deprecated Method
		NodeFileImageResult getInfo(const NodeFileImageResult&, const char* name);///Deprecated Method

		std::string getStr(const NodeFileImageResult&, const char* name)const;///Deprecated Method
		void setStr(const NodeFileImageResult&, const char* name, std::string value);///Deprecated Method
		
		NodeFileImageResult getNode(std::string path);///Deprecated Method
		std::string getValue(std::string path);///Deprecated Method

		static std::string translatePath(FilePath path);///Deprecated Method
		static FilePath translatePath(std::string path);///Deprecated Method
	private:
		int saveFile(const NodeFileImage& node, std::string& output,bool isFirst);///return count string for save
	};
};
#endif
