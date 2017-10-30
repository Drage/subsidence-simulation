
#include "CmdArgs.h"

CmdArgs::CmdArgs()
{
	m_init = false;
}

CmdArgs::CmdArgs(int argc, char **argv)
{
	Init(argc, argv);
}

void CmdArgs::Init(int argc, char **argv)
{
	std::string lastArgName = "";
	
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			std::string name = argv[i] + 1;
			m_args[name] = "";
			lastArgName = name;
		}
		else 
		{
			std::string value = argv[i];
			m_args[lastArgName] = value;
		}
	}
	
	m_init = true;
}
