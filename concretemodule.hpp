#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "basemodule.hpp"


//-----------------------------
// Example: modules definition
//-----------------------------


//-------------------
// Module type no. 1
//-------------------


enum MODULE_TYPE_1
{
	TYPE_1_IMPLEMENTATION_1,
};


//! MODULE TYPE 1
/*!
	Example:
		out:
			vector<int>; 
		in:
			int,
			float,
			string
*/
class ConcreteModuleType1 : public BaseModule<
	/* Out: */	std::vector<int>,
	/* In: */	int,
				float,
				std::string,
				float>
{
private:
	
	RESULT process(std::vector<int> &out, const int &in1, const float &in2, const std::string &str, const float &in4)
	{
		return processImpl(out, in1, in2, str, in4);
	}

public:

	static std::shared_ptr<ConcreteModuleType1> create(MODULE_TYPE_1 type);
	virtual RESULT processImpl(std::vector<int> &out, const int &in1, const float &in2, const std::string &str, const float &in4) = 0;
};


class ModuleType1 : public ConcreteModuleType1
{
public:

	RESULT processImpl(std::vector<int> &out, const int &in1, const float &in2, const std::string &in3, const float &in4)
	{
		out.resize(in1);
		for (size_t i = 0; i < out.size(); i++)
		{
			out[i] = i;
		}
		std::cout << "ModuleType1:\n In1: " << in1
			<< " In2: " << in2
			<< " In3: " << in3
			<< " In4: " << in4
			<< " Out: " << out.size() << std::endl;
		return 0;
	}
};



//-------------------
// Module type no. 2
//-------------------


enum MODULE_TYPE_2
{
	TYPE_2_IMPLEMENTATION_1,
};

//! MODULE TYPE 2
/*!
	Example:
		out:
			vector<string>;
		in:
			vector<int>,
			float,
			string
*/
class ConcreteModuleType2 : public BaseModule<
	/* Out: */	std::vector<std::string>,
	/* In: */	std::vector<int>,
				float,
				std::string>
{
private:

	RESULT process(std::vector<std::string> &out, const std::vector<int> &in1, const float &in2, const std::string &str)
	{
		return processImpl(out, in1, in2, str);
	}

public:

	static std::shared_ptr<ConcreteModuleType2> create(MODULE_TYPE_2 type);
	virtual RESULT processImpl(std::vector<std::string> &out, const std::vector<int> &in1, const float &in2, const std::string &str) = 0;
};

class ModuleType2 : public ConcreteModuleType2
{
public:

	RESULT processImpl(std::vector<std::string> &out, const std::vector<int> &in1, const float &in2, const std::string &in3)
	{
		out.resize(100, "blabla");
		std::cout << "ModuleType2:\n In1: " << in1.size()
			<< " In2: " << in2
			<< " In3: " << in3
			<< " Out: " << out.size() << std::endl;
		return 0;
	}
};



//---------
// Factory
//---------

std::shared_ptr<ConcreteModuleType1> ConcreteModuleType1::create(MODULE_TYPE_1 type)
{
	if (type == TYPE_1_IMPLEMENTATION_1)
		return std::shared_ptr<ConcreteModuleType1>(new ModuleType1());
	return NULL;
}


std::shared_ptr<ConcreteModuleType2> ConcreteModuleType2::create(MODULE_TYPE_2 type)
{
	if (type == TYPE_2_IMPLEMENTATION_1)
		return std::shared_ptr<ConcreteModuleType2>(new ModuleType2());
	return NULL;
}