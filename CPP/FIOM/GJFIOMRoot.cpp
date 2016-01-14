/****************************************************************************
Copyright (c) 2012-2013 grizzly-jr
Copyright (c) 2012-2013 Ivlev Alexander 
****************************************************************************/

#include "GJFIOMRoot.h"
#include <stdarg.h>

#ifdef GRIZZLY_JR_FION_ENABLE_COCOS2D
#include "cocos2d.h"
#endif

using namespace GrizzlyJr;
using namespace FIOM;

#ifdef GRIZZLY_JR_FION_ENABLE_COCOS2D
USING_NS_CC;
#endif

#ifdef GRIZZLY_JR_FION_ENABLE_COCOS2D
static std::string cocos2dGetWriteablePathDefault(std::string relativePath) {
	CCFileUtils::sharedFileUtils()->setPopupNotify(false);
	std::string fullPath = CCFileUtils::sharedFileUtils()->getWriteablePath()+relativePath;
	CCFileUtils::sharedFileUtils()->setPopupNotify(true);
	return fullPath;
}

static std::string cocos2dGetReadOnlyPathDefault(std::string relativePath) {
	CCFileUtils::sharedFileUtils()->setPopupNotify(false);
	const char* fullPath = CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(relativePath.c_str());
	CCFileUtils::sharedFileUtils()->setPopupNotify(true);
	return std::string(fullPath);
}
#else
#define DEFAULT_WRITEABLE_PATH ""
#define DEFAULT_READONLY_PATH ""

static std::string cocos2dDontUseGetWriteablePathDefault(std::string relativePath) {
	return DEFAULT_WRITEABLE_PATH+relativePath;
}

static std::string cocos2dDontUseGetReadOnlyPathDefault(std::string relativePath) {
	return DEFAULT_READONLY_PATH+relativePath;
}

#endif

static unsigned char* getDataFromFile(std::string path,const char* mode,unsigned long* pSize) {
	if( 0x0 == mode || 0 ==path.length() || 0x0 == pSize) {
		return 0x0;
	}

	unsigned char* pBuffer = 0x0;
	*pSize = 0;

	FILE *fp = fopen(path.c_str(), mode);
	if( 0x0 == fp) {
		return 0x0;
	}

	fseek(fp,0,SEEK_END);
	*pSize = ftell(fp);
	fseek(fp,0,SEEK_SET);
	pBuffer = new unsigned char[*pSize];
	*pSize = fread(pBuffer,sizeof(unsigned char), *pSize,fp);
	fclose(fp);

	return pBuffer;
}

GJFIOMRoot GJFIOMRoot::singleton;

GJFIOMRoot::GJFIOMRoot():GJFIOMNode() {
	setType(NODE_DICT);///default
	userFunctionLoad = 0x0;
	userFunctionSave = 0x0;

#ifdef GRIZZLY_JR_FION_ENABLE_COCOS2D
	getWriteablePath = &cocos2dGetWriteablePathDefault;
	getReadOnlyPath = &cocos2dGetReadOnlyPathDefault;
#else
	getWriteablePath = &cocos2dDontUseGetWriteablePathDefault;
	getReadOnlyPath = &cocos2dDontUseGetReadOnlyPathDefault;
#endif
}
GJFIOMRoot::~GJFIOMRoot() {
	GJFIOMNode::~GJFIOMNode();
}

const GJFIOMRoot& GJFIOMRoot::get() {
	return singleton;
}

void GJFIOMRoot::changeWriteablePathFunction(FunctionGetWriteablePath newFunction) {
	if( 0x0 != newFunction) {
		singleton.getWriteablePath = newFunction;
	}
}
void GJFIOMRoot::changeReadOnlyPathFunction(FunctionGetReadOnlyPath newFunction) {
	if( 0x0 != newFunction) {
		singleton.getReadOnlyPath = newFunction;
	}
}

static bool isCorrectPathType(GJFIOMRoot::GJFIOMPathType type) {
	return type >= 0 && type < GJFIOMRoot::GJFIOM_COUNT_PATH_TYPE;
}

#pragma region GJ_FIOM_ROOT_PUBLIC_LOAD_FUNCTIONS
bool GJFIOMRoot::load(std::string path) {
	return load(path,4,GJFIOM_CHANGEABLE_CATALOG,GJFIOM_READ_ONLY_CATALOG,GJFIOM_NET_CATALOG,GJFIOM_USER_CATALOG);
}
bool GJFIOMRoot::load(std::string path,size_t count,GJFIOMPathType typeFirst,...) {
	FunctionForLoadData functionsForLoadData[] = {
		&getDataFromPathChangeable,
		&getDataFromPathReadOnly,
		&getDataFromPathNET,
		singleton.userFunctionLoad
	};

	std::string name = getNameForPath(path);

	va_list types;
	va_start(types,typeFirst);

	GJFIOMPathType type = typeFirst;
	for(size_t i =0; i < count;i++,(type = va_arg(types,GJFIOMPathType))){
		if( !isCorrectPathType(type)) {
			continue;
		}

		FunctionForLoadData loadData = functionsForLoadData[(int)type];
		if( !loadData) {
			continue;
		}

		unsigned long size = 0;
		unsigned char* data = loadData(path,&size);	
		if( load(name.c_str(),data,size)) {
			delete[] data;
			return true;
		}
		delete[] data;
	}
	return false;
}
bool GJFIOMRoot::load(const char* name,const unsigned char* data,unsigned long size) {
	if( 0x0 == name || 0x0 == data || 0 == size) {/// test on correct input data
		return false;
	}

	unsigned char* cpyData = (unsigned char*)malloc(sizeof(unsigned char)*size);
	if( !cpyData) {/// don't memory
		return false;
	}
	cpyData = (unsigned char*)memcpy(cpyData,data,size);

	bool result = loadUnSaveData(name,cpyData,size);
	free(cpyData);
	return result;
}
#pragma endregion

#pragma region GJ_FIOM_ROOT_PUBLIC_SAVE_FUNCTIONS
bool GJFIOMRoot::save(std::string name) {
	return save(name,GJFIOM_CHANGEABLE_CATALOG);
}
bool GJFIOMRoot::save(std::string path,GJFIOMPathType type) {
	std::string name = getNameForPath(path);
	const GJFIOMNode& node = singleton[name];
	if( !node.isCorrect()) {
		return false;
	}
	return save(path,node,type);
}

bool GJFIOMRoot::save(std::string path,const GJFIOMNode& node,GJFIOMPathType type) {
	FunctionForSaveData functionsForSaveData[] = {
		&saveDataByPathChangeable,
		0x0,
		&saveDataByPathNET,
		singleton.userFunctionSave
	};

	if( !isCorrectPathType(type)) {
		return false;
	}

	FunctionForSaveData saveData = functionsForSaveData[(int)type];
	if( !saveData) {
		return false;
	}

	std::string stream;
	stream.clear();
	if( convertNodeOnData(node,stream,0) ) {
		return saveData(path,(const unsigned char*)stream.c_str(),stream.size());
	}
	return false;
}
#pragma endregion

bool GJFIOMRoot::close(std::string name) {
	return singleton.data->childs.remove(name);
}

#pragma region GJ_FIOM_ROOT_UTILS_FUNCTIONS
std::string GJFIOMRoot::removeAllMultiSlash(const char* data) {
	std::string result;
	bool isSlash = false;

	for(; '\0' != *data; ++data) {
		if( *data == '/' || *data == '\\') {
			if( isSlash) { continue; }
			isSlash = true;
		} else {
			isSlash = false;
		}

		result.push_back(*data);
	}
	return result;
}
std::string GJFIOMRoot::getNameForPath(std::string name) {
	const size_t size = name.size();
	const char* data = name.c_str()+size;
	for( size_t i =0; i < size;i++, data--) {
		if( '\\' == *data || '/' == *data) {
			return std::string(data+1);
		}
	}
	return name;
}
#pragma endregion

#pragma region GJ_FIOM_ROOT_STANDART_FUNCTION_LOAD_DATA
unsigned char* GJFIOMRoot::getDataFromPathChangeable(std::string path,unsigned long* sizeData) {
	std::string fullPath = singleton.getWriteablePath(path);
	if( 0 == fullPath.length()) {
		return 0x0;
	}

	std::string realPath = removeAllMultiSlash(fullPath.c_str());
	return getDataFromFile(realPath,"r",sizeData);
}

unsigned char* GJFIOMRoot::getDataFromPathReadOnly(std::string path,unsigned long* sizeData) {
	std::string fullPath = singleton.getReadOnlyPath(path);
	if( 0 == fullPath.length()) {
		return 0x0;
	}

	std::string realPath = removeAllMultiSlash(fullPath.c_str());
	return getDataFromFile(realPath,"r",sizeData);
}

unsigned char* GJFIOMRoot::getDataFromPathNET(std::string,unsigned long*) {
	return 0x0;
}
#pragma endregion

#pragma region GJ_FIOM_ROOT_STANDART_FUNCTION_SAVE_DATA
bool GJFIOMRoot::saveDataByPathChangeable(std::string path,const unsigned char* data,unsigned long sizeData) {
	std::string fullPath = singleton.getWriteablePath(path);
	if( 0 == fullPath.length()) {
		return 0x0;
	}

	std::string realPath = removeAllMultiSlash(fullPath.c_str());
	FILE* file = fopen(realPath.c_str(),"w");
	if( !file) {
		return false;
	}

	if( fwrite(data,sizeof(unsigned char),sizeData,file) == sizeData) {
		fclose(file);
		return true;
	}

	fclose(file);
	return false;
}
bool GJFIOMRoot::saveDataByPathNET(std::string,const unsigned char*,unsigned long) {
	return false;
}
#pragma endregion

bool GJFIOMRoot::loadUnSaveData(const char* name,unsigned char* literals,unsigned long size) {
	size_t count = 0;
	GJFIOMEasyString* words = parsingOnWord(literals,size,count);
	GJFIOMNodeWithWords info = createFIOMUseWords(words,count);
	free(words);

	if( info.node.isCorrect()) {
		singleton[name] = info.node;
		return true;
	}

	return false;
}	

GJFIOMRoot::GJFIOMEasyString* GJFIOMRoot::parsingOnWord(unsigned char* literals, unsigned long size, size_t& count) {
	if( 0x0 == literals || 0 == size) {
		return 0x0;
	}
	static const unsigned char separations[] = " \t\n\r={}[]<>";
	static const unsigned char* wordSaveSeparations = separations+4;

	GJFIOMEasyString* words = (GJFIOMEasyString*)malloc(sizeof(GJFIOMEasyString)*size);
	count = 0;
	words[0].size = 0;
	words[0].word = (char*)literals;

	bool isLineComment = false;
	bool isMultiLineComment = false;

	unsigned long quoteLiteralPositionSub = 0;
	bool isQuote = false;
	bool saveLiteral = false;
	for( unsigned long i =0; i < size; ++i) {
#pragma region PROCESSIONG_QUOTE
		if(isQuote) {
			if( '\\' == literals[i] && '"' == literals[i+1]) {
				literals[i-quoteLiteralPositionSub] = '"';
				quoteLiteralPositionSub++;
				words[count].size++;
				i++;
				continue;
			} else if('"' == literals[i]) {
				isQuote = false;
				goto CREATE_WORD;
			}
			literals[i-quoteLiteralPositionSub] = literals[i];
			words[count].size++;
			continue;
		} else if('"' == literals[i]) {
			quoteLiteralPositionSub = 0;
			isQuote = true;
			goto CREATE_WORD;
		}
#pragma endregion

#pragma region PROCESSING_MULTI_LINE_COMMENT
		if(!isMultiLineComment && '/' == literals[i] && '*' == literals[i+1]) {
			isMultiLineComment = true;
		}
		if(isMultiLineComment && '*' == literals[i] && '/' == literals[i+1]) {
			isMultiLineComment = false;
			++i;
			goto CREATE_WORD;
		}
		if( isMultiLineComment) {
			continue;
		}
#pragma endregion

#pragma region PROCESSING_LINE_COMMENT
		if(!isLineComment && '/' == literals[i] && '/' == literals[i+1]) {
			isLineComment = true;
		}
		if(isLineComment && '\n' == literals[i]) {
			isLineComment = false;
		}
		if( isLineComment) {
			continue;
		}
#pragma endregion

#pragma region PROCESSING_SEPARATION_LITERALS
		for( const unsigned char* ch = separations; 0 != *ch; ch++) {
			if( *ch == literals[i]) {
				if( ch >= wordSaveSeparations) {
					saveLiteral = true;
				}
				goto CREATE_WORD;
			}
		}
#pragma endregion

		words[count].size++;
		continue;
CREATE_WORD:;
#pragma region CREATE_WORD_WITH_LITERAL_IF_NEEDING
		if( words[count].size > 0) {
			count++;
		}
		if( saveLiteral) {
			words[count].size = 1;
			words[count].word = (char*)(literals+i);
			count++;
			saveLiteral = false;
		} 
		words[count].size = 0;
		words[count].word = (char*)(literals+(i+1));
#pragma endregion
	}

	words = (GJFIOMEasyString*)realloc(words,sizeof(GJFIOMEasyString)*count);

	return words;
}

#define TEST_ON_COMPOSITE_LITERAL(LITERAL) ('[' == (LITERAL) || '{' == (LITERAL) || '<' == (LITERAL))
#define SET_TYPE(TYPE,SYMBOL) result.setType(TYPE); ++words; --count; compositeEndLiteral = SYMBOL;
GJFIOMRoot::GJFIOMNodeWithWords GJFIOMRoot::createFIOMUseWords(GJFIOMEasyString* words,size_t count) {
	GJFIOMNode result;
	if( 0 ==count ) {
		result.setType(NODE_NOT_INIT);
		return GJFIOMNodeWithWords(result,words,count);
	}

	char compositeEndLiteral = 0;

	switch (words->word[0]) {
		case '[':	SET_TYPE(NODE_ARRAY,']');	break;
		case '{':	SET_TYPE(NODE_DICT,'}');	break;
		case '<':	SET_TYPE(NODE_SPECIAL,'>');	break;
		default:	result.setType(NODE_DICT);
	}

#pragma region CREATE_CHILDS_AND_VALUE_FOR_ARRAY
	if( result.isArray()) {
		size_t index = 0;
		for(;count > 0;++words,--count) {
			if( TEST_ON_COMPOSITE_LITERAL(words->word[0])) {
				GJFIOMNodeWithWords info = createFIOMUseWords(words,count);
				words = info.words; count = info.count;
				if( info.node.isCorrect()) { result[index++] = info.node; }
				continue;
			}

			if( compositeEndLiteral == words->word[0]) {
				return GJFIOMNodeWithWords(result,words,count);
			}

			result(index++) = std::string(words->word,words->size);
			
		}
	}
#pragma endregion

#pragma region CREATE_CHILDS_AND_VALUE_FOR_DICT_AND_SPECIAL
	if( result.isDict() || result.isSpecial()) {
		GJFIOMEasyString* lastWord = 0x0;
		for(;count > 0;++words,--count) {
			if( lastWord && '=' == words->word[0] && count > 1) {
				GJFIOMEasyString* nextWord = ++words;--count;
				std::string name = std::string(lastWord->word,lastWord->size);
				std::string value = std::string(nextWord->word,nextWord->size);
				result(name) = value;

				lastWord = 0x0;
				continue;
			}

			if( TEST_ON_COMPOSITE_LITERAL(words->word[0]) && lastWord) {
				GJFIOMNodeWithWords info = createFIOMUseWords(words,count);
				words = info.words; count = info.count;
				if( info.node.isCorrect()) { 
					std::string name = std::string(lastWord->word,lastWord->size);
					result[name] = info.node; 
				}
				lastWord = 0x0;
				continue;
			}

			if( compositeEndLiteral == words->word[0]) {
				return GJFIOMNodeWithWords(result,words,count);
			}

			lastWord = words;
		}
	}
#pragma  endregion
	
	return GJFIOMNodeWithWords(result,words,count);
}
#undef TEST_ON_COMPOSITE_SYMBOLS
#undef SET_TYPE

std::string GJFIOMRoot::convertMemoryStringOnFileString(std::string str) {
	bool isQuote = false;

	int pos = -2;
	std::string quote = "\\\"";
	while ( (size_t)(pos = str.find('"',pos+2)) != str.npos ) {
		str.replace(pos,1,quote);
		isQuote = true;
	}

	if( str.find(' ') != str.npos) {
		isQuote = true;
	}

	if( isQuote) {
		str = '"'+str+'"';
	}
	return str;
}

std::string GJFIOMRoot::getTabulation(int tabulation) {
	std::string result;
	for( int i =0; i < tabulation; i++) {
		result += '\t';
	}
	return result;
}

bool GJFIOMRoot::convertNodeOnData(const GJFIOMNode& node,std::string& stream, int tabulation) {
	std::string tabs = getTabulation(tabulation);
	std::string tabsSub = getTabulation(tabulation-1);
	if( node.isArray()) {
		stream += '[';
		char separator = '\n';
		stream += separator;

		size_t size = node.size();
		for( size_t i = 0; i < size;i++) {
			if( node.isChild(i)) {
				stream += tabs;
				convertNodeOnData(node[i],stream,tabulation+1);
			}
			if( node.isValue(i)) {
				stream += tabs;
				stream += convertMemoryStringOnFileString(node(i));
			}
			stream += separator;
		}

		stream += tabsSub;
		stream += ']';
		stream += '\n';
		return true;
	}

	if( node.isSpecial() || node.isDict()) {
		char compositeBeginLiteral = '{';
		char compositeEndLiteral = '}';
		if( node.isSpecial()) {
			compositeBeginLiteral = '<';
			compositeEndLiteral = '>';
		}

		char separator = '\n';
		std::vector<std::string> valueKeys = node.allValuesKeys();
		std::vector<std::string> childKeys = node.allChildsKeys();
		if(  0 == childKeys.size() && valueKeys.size() <= 2) {
			separator = ' ';
			tabs = "";
			tabsSub = "";
		}

		stream += compositeBeginLiteral;
		stream += separator;

		for( size_t i =0; i < valueKeys.size(); i++) {
			stream += tabs;
			stream += convertMemoryStringOnFileString(valueKeys[i]);
			stream += "= ";
			stream += convertMemoryStringOnFileString(node(valueKeys[i]));
			stream += separator;
		}

		for( size_t i =0; i < childKeys.size(); i++) {
			stream += tabs;
			stream += convertMemoryStringOnFileString(childKeys[i]);
			convertNodeOnData(node[childKeys[i]],stream,tabulation+1);
		}
			
		stream += tabsSub;
		stream += compositeEndLiteral;
		stream += '\n';
		return true;
	}
	return false;
}