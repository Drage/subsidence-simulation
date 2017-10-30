
/*
 * @file	Xml.h/.cpp
 * @brief	Parses xml files.
 * @details	Very sensitive to incorrect markeup e.g. missing </> tags.
 *			Can convert to any type with stream operators defined.
 * @author	Matt Drage
 * @date	28/12/2012
 */

#ifndef XML_H
#define XML_H

#include <vector>
#include <string>
#include <sstream>
#include <typeinfo>

class Xml
{
	public:
		// An XML element, defined by <element></element> tags
		struct Element
		{
			std::string name;
			std::string value;
			std::vector<Element*> subElements;

			Element* GetSubElement(const std::string &name);
			
			// Get a value from a path relative to this element
			template<typename T>
			T Get(const std::string &path)
			{
				Element *e = this;
				std::string xPath = path;
				
				// Traverse path
				while (true)
				{
					// Get name of next element in path
					std::string name;
					int slash = xPath.find('/');
					if (slash != std::string::npos)
						name = xPath.substr(0, slash);
					else 
						name = xPath;
					
					// Next element
					e = e->GetSubElement(name);
					
					if (slash == std::string::npos)
						break;
					
					xPath = xPath.substr(slash + 1);
				}
				
				// Strange hack required to return whole string if string has spaces
				// (otherwise only the first word would be returned)
				// "return e->value;" would make more sense, but this is needed to
				// keep the compiler happy
				if (typeid(T) == typeid(std::string))
					return *((T*)((void*)&e->value));
				
				// Convert type from string
				T result;
				std::stringstream ss(e->value);
				ss >> result;
				return result;
			}
		};

		typedef struct Element Element;
		typedef std::vector<Element*> ElementListType;

		// The root namespace element - tags that encapsulate the whole document
		Element root;

		// Constructors & destructors
		Xml();
		Xml(const std::string &filename);
		~Xml();
	
		// Parse a file
		bool Load(const std::string &filename);
	
		// Get value relative to root
		template<typename T>
		T Get(const std::string &path)
		{
			return root.Get<T>(path);
		}

	private:
		void ReadSubElements(Element *current, std::ifstream &file);
		void DeleteSubElements(Element *current);
};

#endif
