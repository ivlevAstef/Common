#ifndef _GRIZZLY_JR_FILE_PATH_H_
#define _GRIZZLY_JR_FILE_PATH_H_

#include <string.h>
#include <stdarg.h>

#define MAX_PATH_DEPTH 8
#define MAX_PATH_NAME_ONE_DEPTH_LENGTH 128
#define MAX_PATH_NAME_LENGTH 512

namespace GrizzlyJr
{
	#pragma warning(disable:4996)
	class FilePath {
	public:
		char p[MAX_PATH_DEPTH][MAX_PATH_NAME_ONE_DEPTH_LENGTH];
		unsigned int count;
	public:
		FilePath() {count = 0;}
		
		FilePath(const char* chars,...) {
			va_list arg_list;
			va_start(arg_list, chars);
			const char* iter = chars;
			
			count = 0;
			while(iter != 0x0) {
				strcpy(p[count], iter);
				count++;
				iter = va_arg(arg_list, const char*);
			};
			
			va_end(arg_list);				
		}
		FilePath(FilePath& path2,const char* chars,...) {
			count = 0;
			for( unsigned int i =0; i < path2.count;i++) {
				this->addPath(path2.p[i]);
			}
			
			va_list arg_list;
			va_start(arg_list, chars);
			const char* iter = chars;
			while(iter != 0x0) {
				strcpy(p[count], iter);
				count++;
				iter = va_arg(arg_list, const char*);
			};
			
			va_end(arg_list);	
		}
		
		FilePath(const FilePath& path2) {
			count = 0;
			for( unsigned int i =0; i < path2.count;i++) {
				this->addPath(path2.p[i]);
			}				
		}
		bool operator<(const FilePath& path2)const {
			if( count < path2.count) {
				return true;
			}
			if( count > path2.count) {
				return false;
			}
			
			for( unsigned int i =0; i < count; i++) {
				int comp = strcmp(p[i], path2.p[i]);
				if( comp < 0) {
					return true;
				}
				if( comp > 0) {
					return false;
				}
			}
			return false;
		}
		void addPath(const char* name) {
			strcpy(p[count], name);
			count++;
		}
		void addPath(const FilePath& path2) {
			for( unsigned int i =0; i < path2.count;i++) {
				this->addPath(path2.p[i]);
			}
		}
	};
	#pragma warning(default:4996)
};
#endif
