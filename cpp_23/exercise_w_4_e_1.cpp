#include <iostream>
#include <string>
#include <variant>
#include <vector>

struct XmlFile
{
    std::string name;
};

struct JsonFile
{
    std::string name;
};

struct Visitor
{
    void operator()(const XmlFile& file)
    {
        std::cout << "Reading XML file \"" + file.name + "\"\n";
    }

    void operator()(const JsonFile& file)
    {
        std::cout << "Reading JSON file \"" + file.name + "\"\n";
    }
    void operator()(...) {
        
    }
};

int main()
{
    std::vector<std::variant<XmlFile, JsonFile>> files =
            {
                    XmlFile{"a.xml"},
                    XmlFile{"b.xml"},
                    JsonFile{"c.json"},
                    XmlFile{"d.xml"},
            };

    for (const auto& file : files)
        std::visit(Visitor(), file);
}

