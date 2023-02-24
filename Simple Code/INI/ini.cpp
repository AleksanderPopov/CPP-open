#include "ini.h"

using Section = std::unordered_map<std::string, std::string>;

ini::Section& ini::Document::AddSection(std::string name) {
    if (sections_.count(name) != 0) {        
        return sections_.at(name);
    }
    else
    {       
        sections_.emplace(name, ini::Section{});        
        return sections_.at(name);
    }	
}

const ini::Section& ini::Document::GetSection(const std::string& name) const {
    static ini::Section empty_section{};   
    if (sections_.count(name)==0 || sections_.at(name).empty()) {      
        return empty_section;
    }
    else {      
        return sections_.at(name);
    }
}

std::size_t ini::Document::GetSectionCount() const {
    return sections_.size();
}


ini::Document ini::Load(std::istream& input) {
	ini::Document result;
    std::string section_name;

    for (std::string temp; std::getline(input, temp);) {
             
        int section_start = temp.find_first_of("[");

        if (section_start >=0) {
            int section_end = temp.find_first_of("]")-1;
            section_name = temp.substr(section_start +1, section_end - section_start);
            result.AddSection(section_name);;           
        }

        if (temp.find("=")!=std::string::npos) {
            int ptr = temp.find("=");

            int l_first = temp.find_first_not_of(" ");
            int l_second = temp.find_last_not_of(" ",ptr-1);           
            int r_first = temp.find_first_not_of(" ",ptr+1);
            int r_second = temp.find_last_not_of(" "); 

            std::string key = temp.substr(l_first, l_second - l_first+1);
            std::string vol = temp.substr(r_first, r_second - r_first+1);

            result.AddSection(section_name).insert({ key,vol });
        }
    }
	return result;
}