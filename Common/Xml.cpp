
#include "Xml.h"
#include <fstream>

Xml::Xml()
{
}

Xml::Xml(const std::string &filename)
{
	Load(filename);
}

Xml::~Xml()
{
	DeleteSubElements(&root);
}

bool Xml::Load(const std::string &filename)
{
	// Append extension
	std::string filePath = filename;
	if (filename.find(".") == std::string::npos)
		filePath += ".xml";
	
	// Open file
	std::ifstream file(filePath.c_str());

	// Check file is open
	if (!file.is_open()) 
        return false;

	// Read all in root namespace
	char buffer[256] = {0};
	file.getline(buffer, sizeof(buffer), '<');
	file.getline(buffer, sizeof(buffer), '>');
	root.name = std::string(buffer);
	ReadSubElements(&root, file);
	return true;
}

void Xml::ReadSubElements(Element *current, std::ifstream &file)
{
	std::string name, value;
	char buffer[256] = {0};

	while (true)
	{
		file.getline(buffer, sizeof(buffer), '<');
		if (file.peek() == '/')
		{
			current->value = std::string(buffer);
			file.getline(buffer, sizeof(buffer), '>');
			break;
		}
		else
		{
			Element* subElement = new Element();
			file.getline(buffer, sizeof(buffer), '>');
			subElement->name = std::string(buffer);
			current->subElements.push_back(subElement);
			ReadSubElements(subElement, file);
		}
	}
}

void Xml::DeleteSubElements(Element *current)
{
	for (ElementListType::iterator i = current->subElements.begin(); i != current->subElements.end(); i++)
	{
		DeleteSubElements(*i);
		delete *i;
	}
}

Xml::Element* Xml::Element::GetSubElement(const std::string &name)
{
	for (ElementListType::const_iterator i = subElements.begin(); i != subElements.end(); i++)
	{
		if ((*i)->name == name)
			return *i;
	}
	return NULL;
}
