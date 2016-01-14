//#include "GJFIOMHashMap.h"
//
//using namespace GrizzlyJr;
//using namespace FIOM;
//
//size_t GJFIOMHashMap::default_size_hash_map = 32;
//
//GJFIOMHashMap::GJFIOMHashMap() {
//	pairs = 0x0; 
//	size_array = 0; 
//	count_pairs = 0;
//	allocMemory(default_size_hash_map);
//}
//
//void GJFIOMHashMap::set(type_key key,type_value value) {
//	set(key,value,pairs,size_array);
//	count_pairs++;
//	reallocIfNeed();
//}
//void GJFIOMHashMap::set(size_t index,type_value value) {
//	if( 0x0 == pairs[index]) {
//		pairs[index] = (GJFIOMHashMapPair*)malloc(sizeof(GJFIOMHashMapPair));
//	}
//	pairs[index]->value = value;
//	pairs[index]->next = 0x0;
//
//	count_pairs++;
//	reallocIfNeed();
//}
//
//bool GJFIOMHashMap::is(type_key key) {
//	size_t index = getHash(key,size_array);
//	GJFIOMHashMapPair* pair = pairs[index];
//	while(0x0 != pair) {
//		if( pair->key == key) {
//			return true;
//		}
//		pair = pair->next;
//	}
//	return false;
//}
//bool GJFIOMHashMap::is(size_t index) {
//	return (0x0 != pairs[index]);
//}
//
//type_value GJFIOMHashMap::get(type_key key) {
//	size_t index = getHash(key,size_array);
//	GJFIOMHashMapPair* pair = pairs[index];
//	while(0x0 != pair) {
//		if( pair->key == key) {
//			return pair->value;
//		}
//		pair = pair->next;
//	}
//	return type_value();
//}
//type_value GJFIOMHashMap::get(size_t index) {
//	if( 0x0 != pairs[index]) {
//		return pairs[index]->value;
//	}
//	return type_value();
//}
//
//void GJFIOMHashMap::remove(type_key key) {
//	size_t index = getHash(key,size_array);
//	
//	GJFIOMHashMapPair* pair = pairs[index];
//	GJFIOMHashMapPair* old = 0x0;
//
//	while(0x0 != pair) {
//		if( pair->key == key) {
//			if( old) {
//				old->next = pair->next;
//			} else {
//				pairs[index] = 0x0;
//			}
//			free(pair);
//			count_pairs--;
//			return;
//		}
//
//		old = pair;
//		pair = pair->next;
//	}
//}
//void GJFIOMHashMap::remove(size_t index) {
//	if( 0x0 == pairs[index]) {
//		return;
//	}
//	free(pairs[index]);
//	pairs[index] = 0x0;
//}
//
//size_t GJFIOMHashMap::getHash(type_key key,size_t size_array) {
//	const size_t size = MIN(4,key.size());
//	size_t value = 0;
//	size_t mult = 1;
//	for( size_t i =0; i < size; i++) {
//		size_t vI = key[i];
//		if( vI >= '0') {
//			vI -= '0';
//		}
//
//		value += vI*mult;
//		mult*=10;
//	}
//	return value%size_array;
//}
//
//void GJFIOMHashMap::reallocIfNeed() {
//	if( count_pairs > size_array/2) {
//		allocMemory(size_array*2);
//	}
//}
//
//void GJFIOMHashMap::allocMemory(size_t size) {
//	size_t newSize_array = size;
//	size_t newCount_pairs = 0;
//	GJFIOMHashMapPair** newPairs = (GJFIOMHashMapPair**)calloc(newSize_array,sizeof(GJFIOMHashMapPair*));
//	assert(0x0 != newPairs);
//
//	for( size_t i =0; i < size_array; i++) {
//		GJFIOMHashMapPair* pair = pairs[i];
//		while( 0x0 != pair) {
//			set(pair->key,pair->value,newPairs,newSize_array);
//			newCount_pairs++;
//			pair = pair->next;
//		}
//	}
//
//	freeMemory();
//
//	pairs = newPairs;
//	size_array = newSize_array;
//	count_pairs = newCount_pairs;
//}
//
//void GJFIOMHashMap::freeMemory() {
//	for( size_t i = 0; i < size_array; i++) {
//		GJFIOMHashMapPair* pair = pairs[i];
//		while( 0x0 != pair) {
//			GJFIOMHashMapPair* next = pair->next;
//			free(pair);
//			pair = next;
//		}
//	}
//	if( 0x0 != pairs) {
//		free(pairs);
//		pairs = 0x0;
//	}
//	size_array = 0;
//	count_pairs = 0;
//}
//
//void GJFIOMHashMap::set(type_key key,type_value value,GJFIOMHashMapPair** pairs,size_t size_array) {
//	size_t index = getHash(key,size_array);
//	GJFIOMHashMapPair* pair = pairs[index];
//	GJFIOMHashMapPair* old = 0x0;
//
//	while(0x0 != pair) {
//		if( pair->key == key) {
//			pair->value = value;
//			return;
//		}
//
//		old = pair;
//		pair = pair->next;
//	}
//
//	pair = (GJFIOMHashMapPair*)malloc(sizeof(GJFIOMHashMapPair));
//	pair->key = key;
//	pair->value = value;
//	pair->next = 0x0;
//	if( 0x0 != old) {
//		old->next = pair;
//	}
//}
//
//GJFIOMHashMap::~GJFIOMHashMap() {
//	freeMemory();
//}