
/*
 * @file	CmdArgs.h/.cpp
 * @brief	Allows obtainments of command line arguments by name.
 * @details	Should be specified on the terminal like ./program -arg_name arg_value
 * @author	Matt Drage
 * @date	11/01/2013
 */

#ifndef CMDARGS_H
#define CMDARGS_H

#include <map>
#include <string>
#include <sstream>
#include <typeinfo>

class CmdArgs
{
	typedef std::map<std::string, std::string> ArgMapType;
	
	public:
		// Constructors
		CmdArgs();
		CmdArgs(int argc, char **argv);
	
		// Parse command line arguments
		void Init(int argc, char **argv);
	
		// Set defualt value of arguments
		template<typename T>
		void SetDefault(const std::string &arg, T value)
		{
			// Ignore if value is already set
			if (m_init)
			{
				ArgMapType::iterator i = m_args.find(arg);
				if (i != m_args.end())
					return;
			}
			
			// Make strings include spaces
			if (typeid(T) == typeid(std::string))
			{
				m_args[arg] = value;
				return;
			}
			
			// Convert to string
			std::stringstream ss;
			ss << value;
			m_args[arg] = ss.str();
		}
	
		// Get command line arguments
		template<typename T>
		T Get(const std::string &arg)
		{
			ArgMapType::iterator i 
			= m_args.find(arg);
			if (i == m_args.end())
				return 0;
			
			// Ugly hack to make strings return the whole string including spaces
			if (typeid(T) == typeid(std::string))
				return *((T*)((void*)&i->second));
			
			// Convert type from string
			T result;
			std::stringstream ss(i->second);
			ss >> result;
			return result;
		}

	private:
		bool m_init;
		ArgMapType m_args;
};

#endif
