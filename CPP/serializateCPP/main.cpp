#include "infoserializable.h"
#include "serializate.h"

class VT
{
public:
	float x,y,z;
};

class A: public Serializable::Info
{
public:
	std::string str;
	const stdext::hash_map<std::string,Serializable::InformationObject*> init()
	{ 
		stdext::hash_map<std::string,Serializable::InformationObject*> p;
		Serializable::Convert<std::string>( p, str, "Astr");
		return p;
	}
	const std::string ThisName(){ return "AClass";}
};
class B: public A
{
public:
	float parB;
	const stdext::hash_map<std::string,Serializable::InformationObject*> init()
	{ 
		stdext::hash_map<std::string,Serializable::InformationObject*> p;
		Serializable::Convert<std::string>( p, str, "Bstr");
		Serializable::Convert<float>( p, parB, "parB");
		return p;
	}
	const std::string ThisName(){ return "BClass";}
};
class C: public A
{
public:
	int parC;
	const stdext::hash_map<std::string,Serializable::InformationObject*> init()
	{ 
		stdext::hash_map<std::string,Serializable::InformationObject*> p;
		Serializable::Convert<std::string>( p, str, "Cstr");
		Serializable::Convert<int>( p, parC, "parC");
		return p;
	}
	const std::string ThisName(){ return "CClass";}
};

/*class MySerializate: public Serializable::InformationObject
{
public:
	void load(const Serializable::Block& block)
	{
		
	}
	void save( std::ofstream& output, unsigned int countTab)
	{
		
	}
}; */

class VT2: public Serializable::Info
{
public:
	float x,y,z;
	char ch;
	bool bl;
	std::vector<A*> arr;
	std::vector<std::list<double>> arr2;
	A* aaa;
	stdext::hash_map<std::string, int> map;
	VT2()
	{
		aaa = 0;
		x = 0;
		y = 0;
		z = 0;
	}
	~VT2()
	{
		delete aaa;
		std::vector<A*>::iterator iter = arr.begin();
		for( ; iter != arr.end(); iter++)
		{
			delete *iter;
		}
		arr.clear();
	}
	const stdext::hash_map<std::string,Serializable::InformationObject*> init()
	{ 
		stdext::hash_map<std::string,Serializable::InformationObject*> p;
		Serializable::Convert<float>( p, x, "x");
		Serializable::Convert<float>( p, y, "y");
		Serializable::Convert<float>( p, z, "z");
		Serializable::Convert<float*>( p, f, "f");
		Serializable::Convert<char>( p, ch, "char");
		Serializable::Convert<bool>( p, bl, "bool");
		Serializable::Convert<std::vector<std::list<double>>>( p, arr2, "array2");
		Serializable::Convert<stdext::hash_map<std::string, int>>( p, map, "map");

		Serializable::Convert<std::vector<A*>>(p, arr, "array");
		Serializable::Convert<A*>( p, aaa, "aaa");
		// нужно для поддержки полиморфизма
		Serializable::Convert<A,A>();// необязательно
		Serializable::Convert<A,B>();
		Serializable::Convert<A,C>();
		return p;
	}
	const std::string ThisName(){ return "VT2";}

};

int main(void)
{
	VT2 test;
	//Serializable::Load("test.load", test);
	//Serializable::Save("test.save", test);
	return 0;
}