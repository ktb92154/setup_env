#ifndef CFG_DATA_H_
#define CFG_DATA_H_

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/signals2.hpp>

class cfg_data {
public:
	cfg_data(std::string); // This is the constructor declaration
	~cfg_data(void); // This is the destructor declaration

	//std::wstring cfg_data::getw(std::string);
	std::wstring cfg_data::wget(std::string);
	std::string cfg_data::get(std::string);

private:
	void init(std::string);
	boost::property_tree::ptree m_pt;
};

#endif  CFG_DATA_H_