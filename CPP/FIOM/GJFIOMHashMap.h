/****************************************************************************
Copyright (c) 2012-2013 grizzly-jr
Copyright (c) 2012-2013 Ivlev Alexander 
****************************************************************************/

#ifndef _COMPANY_GRIZZLY_JR__PACKAGE_FIOM__CLASS_HASH_MAP_H_
#define _COMPANY_GRIZZLY_JR__PACKAGE_FIOM__CLASS_HASH_MAP_H_

#include <string>
#include <vector>
#include <assert.h>

namespace GrizzlyJr {
	namespace FIOM {

		class GJFIOMNode;
		template<class type_value>///key = std::string
		class GJFIOMHashMap {
		public:
			typedef std::string type_key;
			static size_t default_size_hash_map;
		private:
			struct GJFIOMHashMapPair {
				type_value value;
				type_key key;
				GJFIOMHashMapPair* next;
			};
			GJFIOMHashMapPair** pairs;
			size_t size_array;
			size_t count_pairs;
		public:
			GJFIOMHashMap() {
				pairs = 0x0; 
				size_array = 0; 
				count_pairs = 0;
				allocMemory(default_size_hash_map);
			}

			void copy(GJFIOMHashMap<type_value>& data) {
				if( this == &data) {
					return;
				}

				freeMemory();

				size_array = data.size_array;
				count_pairs = data.count_pairs;
				pairs = new GJFIOMHashMapPair*[size_array]();
				assert(0x0 != pairs);

				for( size_t i =0; i < size_array; i++) {
					pairs[i] = 0x0;
				}

				for( size_t i =0; i < size_array; i++) {
					GJFIOMHashMapPair* pair = data.pairs[i];
					while( 0x0 != pair) {
						if( "" == pair->key) {
							if( 0x0 == pairs[i]) {
								pairs[i] = new GJFIOMHashMapPair();
							}
							pairs[i]->value = pair->value;
							pairs[i]->next = 0x0;
						} else {
							set(pair->key,pair->value,pairs,size_array);
						}
						pair = pair->next;
					}
				}			
			}

			size_t size() {
				return count_pairs;
			}

			void set(type_key key,type_value& value) {
				set(key,value,pairs,size_array);
				count_pairs++;
				reallocIfNeed();
			}
			bool is(type_key key) {
				size_t index = getHash(key,size_array);
				GJFIOMHashMapPair* pair = pairs[index];
				while(0x0 != pair) {
					if( pair->key == key) {
						return true;
					}
					pair = pair->next;
				}
				return false;
			}
			type_value& get(type_key key) {
				bool one_cicle = true;
				RESTART_GET_FUNCTION:;

				size_t index = getHash(key,size_array);
				GJFIOMHashMapPair* pair = pairs[index];
				while(0x0 != pair) {
					if( pair->key == key) {
						return pair->value;
					}
					pair = pair->next;
				}

				if( one_cicle) {
					type_value value;
					set(key,value);
					one_cicle = false;
					goto RESTART_GET_FUNCTION;
				}
				assert(true);
			}
			bool remove(type_key key) {
				size_t index = getHash(key,size_array);

				GJFIOMHashMapPair* pair = pairs[index];
				GJFIOMHashMapPair* old = 0x0;

				while(0x0 != pair) {
					if( pair->key == key) {
						if( old) {
							old->next = pair->next;
						} else {
							pairs[index] = 0x0;
						}
						delete pair;
						count_pairs--;
						return true;
					}

					old = pair;
					pair = pair->next;
				}
				return false;
			}

			std::vector<type_key> getAllKeys() {
				std::vector<type_key> result;
				if( 0 == count_pairs) {
					return result;
				}
				result.resize(count_pairs);
				type_key* iter = &result[0];

				for( size_t i =0; i < size_array;i++) {
					GJFIOMHashMapPair* pair = pairs[i];
					while(0x0 != pair) {
						(*(iter++)) = pair->key;
						pair = pair->next;
					}
				}
				
				return result;
			}

			~GJFIOMHashMap() {
				freeMemory();
				allocMemory(default_size_hash_map);
			}
		//private:
			friend class GJFIOMNode;

			void set(size_t index,type_value value) {
				if( index >= size_array ) {
					allocMemory(index+1);
				}
				if( 0x0 == pairs[index]) {
					pairs[index] = new GJFIOMHashMapPair();
				}
				pairs[index]->value = value;
				pairs[index]->next = 0x0;

				count_pairs++;
				reallocIfNeed();
			}
			bool is(size_t index) {
				return (index < size_array && 0x0 != pairs[index]);
			}
			type_value get(size_t index) {
				if( index < size_array && 0x0 != pairs[index]) {
					return pairs[index]->value;
				}
				return type_value();
			}
			void remove(size_t index) {
				if( index >= size_array || 0x0 == pairs[index]) {
					return;
				}
				delete pairs[index];
				pairs[index] = 0x0;
			}

			void clean() {
				freeMemory();
			}

		private:
			static GJFIOMHashMapPair* set(type_key key,type_value& value,GJFIOMHashMapPair** pairs,size_t size_array) {
				size_t index = getHash(key,size_array);
				GJFIOMHashMapPair* pair = pairs[index];
				GJFIOMHashMapPair* old = 0x0;

				while(0x0 != pair) {
					if( pair->key == key) {
						pair->value = value;
						return pair;
					}

					old = pair;
					pair = pair->next;
				}

				pair = new GJFIOMHashMapPair();
				pair->key = key;
				pair->value = value;
				pair->next = 0x0;
				if( 0x0 != old) {
					old->next = pair;
				} else {
					pairs[index] = pair;
				}
				return pair;
			}

#define MIN(a,b) ((a)<(b)?(a):(b))
			static size_t getHash(type_key key,size_t size_array) {
				const size_t size = MIN(4,key.size());
				size_t value = 0;
				size_t mult = 1;
				for( size_t i =0; i < size; i++) {
					size_t vI = key[i];
					if( vI >= '0') {
						vI -= '0';
					}

					value += vI*mult;
					mult*=10;
				}
				return value%size_array;
			}
#undef MIN

			void reallocIfNeed() {
				if( count_pairs >= size_array/2) {
					allocMemory(size_array*2);
				}
			}

			void allocMemory(size_t size) {
				size_t newSize_array = size;
				size_t newCount_pairs = 0;
				GJFIOMHashMapPair** newPairs = new GJFIOMHashMapPair*[newSize_array]();
				assert(0x0 != newPairs);

				for( size_t i =0; i < newSize_array; i++) {
					newPairs[i] = 0x0;
				}

				for( size_t i =0; i < size_array; i++) {
					GJFIOMHashMapPair* pair = pairs[i];
					while( 0x0 != pair) {
						if( "" == pair->key) {
							if( 0x0 == newPairs[i]) {
								newPairs[i] = new GJFIOMHashMapPair();
							}
							newPairs[i]->value = pair->value;
							newPairs[i]->next = 0x0;
						} else {
							set(pair->key,pair->value,newPairs,newSize_array);
						}
						newCount_pairs++;
						pair = pair->next;
					}
				}

				freeMemory();

				pairs = newPairs;
				size_array = newSize_array;
				count_pairs = newCount_pairs;
			}

			void freeMemory() {
				for( size_t i = 0; i < size_array; i++) {
					GJFIOMHashMapPair* pair = pairs[i];
					while( 0x0 != pair) {
						GJFIOMHashMapPair* next = pair->next;
						delete pair;
						pair = next;
					}
				}
				if( 0x0 != pairs) {
					delete[] pairs;
					pairs = 0x0;
				}
				size_array = 0;
				count_pairs = 0;
			}
		};

		template<typename type_value> size_t GJFIOMHashMap<type_value>::default_size_hash_map = 2;
	};
};

#endif