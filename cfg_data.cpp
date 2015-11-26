/*
1. Requires Boost to be installed (currently located at: W:\!Amazing_Tools\boost_1_58_0)
2. Compile Boost (boostrap.bat)
3. Add Boost to Visual Studio Project
a. Configuration Properties -> VC++ Directories -> Include Directories -> W:\!Amazing_Tools\boost_1_58_0\stage\lib;$(IncludePath)
b. Configuration Properties -> C/C++ -> General -> Additional Include Directories -> W:\!Amazing_Tools\boost_1_58_0\
4. Include config.ini containing the following inside Debug folder
[Section1]
Value1 = test1
Value2 = test2
[Section2]
*/

#include <iostream>
#include "cfg_data.h"


// std::wstring -> std::string
std::string wstrTostr(std::wstring wStr) {
	std::string str = "";
	// TODO: Add log with log level 9
	return str.assign(wStr.begin(), wStr.end());
}

// std::string -> std::wstring
std::wstring strTowstr(std::string str) {
	std::wstring wStr = L"";
	// TODO: Add log with log level 9
	return wStr.assign(str.begin(), str.end());
}

// Class Constructor
cfg_data::cfg_data()
{
	//cfg_data::save("test.ini", "a.value", "3"); //ONLY A TEST - REMOVE
}

cfg_data::cfg_data(std::string cfg_file)
{
	//cfg_data::save("test.ini", "a.value", "3"); //ONLY A TEST - REMOVE
	init(cfg_file);
}

void cfg_data::init(std::string cfg_file)
{
	try
	{
		boost::property_tree::ini_parser::read_ini("config.ini", m_pt);
		std::cout << "Reading cfg file: " << cfg_file << std::endl;
	}
	catch (std::exception const& ex) {
		std::cerr << ("Caught exception: %s", ex.what()) << std::endl;
		std::cin;
		std::terminate();
		//exit(0); // does not perform cleanup
	}
}

/*cfg_data::~cfg_data(void)
{

}*/

// Read CFG, return as string
std::string cfg_data::get(std::string cfg_value)
{
	return m_pt.get<std::string>(cfg_value);
}

// Read CFG, return as wstring
std::wstring cfg_data::wget(std::string cfg_value)
{
	return strTowstr(m_pt.get<std::string>(cfg_value));
}

// Write CFG -- IN PROGRESS! INCOMPLETE!
// Example: cfg_data::save("test.ini", "a.value", "2");
// Creates: test.ini 
//				[a]
//				value=2
void cfg_data::save(std::string file, std::string section, std::string value)
{
	//if does not file exist
	try
	{
		boost::property_tree::ini_parser::read_ini(file, m_pt);
		std::cout << green << "Reading cfg file: " << file << white << std::endl;
		// Updates section with new value
		m_pt.put(section, value);
		// Writes to file, or creats new file with section and value
		boost::property_tree::ini_parser::write_ini(file, m_pt);

	}
	catch (std::exception const& ex) {
		std::cout << red << file << " not found" << white << std::endl;
		// Updates section with new value
		m_pt.put(section, value);
		// Writes to file, or creats new file with section and value
		boost::property_tree::ini_parser::write_ini(file, m_pt);
		std::cout << green << " Created new cfg file: " << file << white << std::endl;
	}

}

// Write CFG -- IN PROGRESS! INCOMPLETE!
// Example: cfg_data::save("test.ini", "a.value", "2");
// Creates: test.ini 
//				[a]
//				value=2
/*bool cfg_data::save(std::string section, std::string value)
{
	//if does not file exist
	std::string test2 = "test2.ini"; // ***** Goal is to replace this with a private variable defined at constructor
	try
	{
		boost::property_tree::ini_parser::read_ini(test2, m_pt);
		std::cout << green << "Reading cfg file: " << test2 << white << std::endl;
		// Updates section with new value
		m_pt.put(section, value);
		// Writes to file, or creats new file with section and value
		boost::property_tree::ini_parser::write_ini(test2, m_pt);

	}
	catch (std::exception const& ex) {
		std::cout << red << test2 << " not found" << white << std::endl;
		// Updates section with new value
		m_pt.put(section, value);
		// Writes to file, or creats new file with section and value
		boost::property_tree::ini_parser::write_ini(test2, m_pt);
		std::cout << green << " Created new cfg file: " << test2 << white << std::endl;
	}

	return true;
}*/