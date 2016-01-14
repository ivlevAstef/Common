/****************************************************************************
Copyright (c) 2012-2013 grizzly-jr
Copyright (c) 2012-2013 Ivlev Alexander 
****************************************************************************/

#ifndef _COMPANY_GRIZZLY_JR__PACKAGE_FIOM__CLASS_VALUE_H_
#define _COMPANY_GRIZZLY_JR__PACKAGE_FIOM__CLASS_VALUE_H_

#include <string>

namespace GrizzlyJr {
	namespace FIOM {
		class GJFIOMValue {
		private:
			struct GJFIOMValueInformation {
				char* str;
				size_t countSymbols;
				GJFIOMValueInformation();
				GJFIOMValueInformation(const GJFIOMValueInformation& copy);
				void copy(const GJFIOMValueInformation& copy);
				~GJFIOMValueInformation();
				void setString(const char* newstr);
				bool isEqual(const char* str2);
			};
			GJFIOMValueInformation* value;
			size_t* retainCount;
		public:
			GJFIOMValue();
			GJFIOMValue(const GJFIOMValue& copy);
			~GJFIOMValue();

			operator std::string()const;
			operator const char*()const;
			operator char()const;
			operator short()const;
			operator int()const;
			operator long long int()const;
			operator unsigned int()const;
			operator float()const;
			operator double()const;
			operator bool()const;

			const GJFIOMValue& operator=(const GJFIOMValue& copy)const;
			const GJFIOMValue& operator=(const std::string value)const;
			const GJFIOMValue& operator=(const char* value)const;
			const GJFIOMValue& operator=(const short value)const;
			const GJFIOMValue& operator=(const int value)const;
			const GJFIOMValue& operator=(const long long int value)const;
			const GJFIOMValue& operator=(const unsigned int value)const;
			const GJFIOMValue& operator=(const float value)const;
			const GJFIOMValue& operator=(const double value)const;
			const GJFIOMValue& operator=(const bool value)const;

			inline bool isCorrect()const { return 0x0 != value->str;}
		};
	};
};



#endif