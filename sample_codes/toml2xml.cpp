#include <algorithm>
#include <fstream>

#include <toml/toml.hpp>

namespace
{
	//
	class toml_traversable
	{
	public:
		~toml_traversable()
		{
		}

	public:
		virtual void start_array(const std::string &name) = 0;
		virtual void end_array(const std::string &name) = 0;
		virtual void start_table(const std::string &name) = 0;
		virtual void end_table(const std::string &name) = 0;
		virtual void boolean_value(const std::string &key, bool value) = 0;
		virtual void integer_value(const std::string &key, const boost::int64_t &value) = 0;
		virtual void floating_value(const std::string &key, double value) = 0;
		virtual void string_value(const std::string &key, const std::string &value) = 0;
		virtual void date_value(const std::string &key, const boost::gregorian::date &value) = 0;
		virtual void time_value(const std::string &key, const boost::posix_time::time_duration &value) = 0;
		virtual void local_datetime_value(const std::string &key, const boost::posix_time::ptime &value) = 0;
		virtual void offset_datetime_value(const std::string &key, const boost::local_time::local_date_time &value) = 0;
	};

	//
	class toml2xml_traverse : public toml_traversable
	{
	private:
		std::ostream &out;

	public:
		explicit toml2xml_traverse(std::ostream &o) : out(o)
		{
			out << "<?xml version=\"1.0\" encoding=\"utf8\" standalone=\"yes\"?>\n";
		}

		virtual ~toml2xml_traverse()
		{
		}

	private:
		toml2xml_traverse(const toml2xml_traverse &);
		toml2xml_traverse(const toml2xml_traverse &&);
		toml2xml_traverse &operator=(const toml2xml_traverse &);

	public:
		virtual void start_array(const std::string &name)
		{
			out << "<array name=\"" << name << "\">\n";
		}

		virtual void end_array(const std::string &name)
		{
			out << "</array>\n";
		}

		virtual void start_table(const std::string &name)
		{
			out << "<table name=\"" << name << "\">\n";
		}

		virtual void end_table(const std::string &name)
		{
			out << "</table>\n";
		}

		virtual void boolean_value(const std::string &key, bool value)
		{
			out << "<item key=\"" << key << "\" value=\"" << (value ? "true" : "false") << "\"/>\n";
		}

		virtual void integer_value(const std::string &key, const boost::int64_t &value)
		{
			out << "<item key=\"" << key << "\" value=\"" << value << "\"/>\n";
		}

		virtual void floating_value(const std::string &key, double value)
		{
			out << "<item key=\"" << key << "\" value=\"" << value << "\"/>\n";
		}

		virtual void string_value(const std::string &key, const std::string &value)
		{
			out << "<item key=\"" << key << "\" value=\"" << value << "\"/>\n";
		}

		virtual void date_value(const std::string &key, const boost::gregorian::date &value)
		{
			out << "<item key=\"" << key << "\" value=\"" << value << "\"/>\n";
		}

		virtual void time_value(const std::string &key, const boost::posix_time::time_duration &value)
		{
			out << "<item key=\"" << key << "\" value=\"" << value << "\"/>\n";
		}

		virtual void local_datetime_value(const std::string &key, const boost::posix_time::ptime &value)
		{
			out << "<item key=\"" << key << "\" value=\"" << value << "\"/>\n";
		}

		virtual void offset_datetime_value(const std::string &key, const boost::local_time::local_date_time &value)
		{
			out << "<item key=\"" << key << "\" value=\"" << value << "\"/>\n";
		}
	};

	//
	void traverse_toml_array(const std::string& key, const toml::array &array, toml_traversable &handler);
	void traverse_toml_table(const std::string& key, const toml::table &table, toml_traversable &handler);

	//
	void traverse_toml_values(const std::string &key, const toml::value &value, toml_traversable& handler)
	{
		switch (value.which())
		{
		case toml::value::boolean_tag:
			handler.boolean_value(key, toml::get<bool>(value));
			break;

		case toml::value::integer_tag:
			handler.integer_value(key, toml::get<boost::int64_t>(value));
			break;

		case toml::value::float_tag:
			handler.floating_value(key, toml::get<double>(value));
			break;

		case toml::value::string_tag:
			handler.string_value(key, toml::get<std::string>(value));
			break;

		case toml::value::date_tag:
			handler.date_value(key, toml::get<boost::gregorian::date>(value));
			break;

		case toml::value::time_tag:
			handler.time_value(key, toml::get<boost::posix_time::time_duration>(value));
			break;

		case toml::value::local_datetime_tag:
			handler.local_datetime_value(key, toml::get<boost::posix_time::ptime>(value));
			break;

		case toml::value::offset_datetime_tag:
			handler.offset_datetime_value(key, toml::get<boost::local_time::local_date_time>(value));
			break;

		case toml::value::array_tag:
			traverse_toml_array(key, toml::get<toml::array>(value), handler);
			break;

		case toml::value::table_tag:
			traverse_toml_table(key, toml::get<toml::table>(value), handler);
			break;

		default:
			break;
		}
	}

	//
	void traverse_toml_array(const std::string& key, const toml::array &array, toml_traversable &handler)
	{
		handler.start_array(key);

		for(auto itr = array.begin(), end = array.end(); itr != end; itr++)
		{
			traverse_toml_values(key, *itr, handler);
		}

		handler.end_array(key);
	}

	//
	void traverse_toml_table(const std::string& key, const toml::table &table, toml_traversable &handler)
	{
		handler.start_table(key);

		for (auto itr = table.begin(), end = table.end(); itr != end; itr++)
		{
			traverse_toml_values(itr->first, itr->second, handler);
		}

		handler.end_table(key);
	}

	//
	toml::table parse_toml(const char *filename)
	{
		std::ifstream stream(filename);

		// for Windows
		try
		{
			char utf8_bom[] = {
				static_cast<char>(0xEF),
				static_cast<char>(0xBB),
				static_cast<char>(0xBF) };
			char bom_buff[_countof(utf8_bom)] = {};

			stream.read(bom_buff, _countof(bom_buff));

			if ((utf8_bom[0] != bom_buff[0]) ||
				(utf8_bom[1] != bom_buff[1]) ||
				(utf8_bom[2] != bom_buff[2]))
			{
				stream.seekg(0, std::ios::beg);
			}

			return toml::parse(stream);
		}
		catch (const toml::bad_get& e)
		{
			std::cout<<e.what()<<std::endl;

			return toml::table();
		}
		catch (const std::exception& e)
		{
			std::cout<<e.what()<<std::endl;

			return toml::table();
		}
	}

	//
	void toml2xml(const char *filename)
	{
		auto root = parse_toml(filename);

		if(root.empty())
		{
			return;
		}

		toml2xml_traverse handler(std::cout);

		traverse_toml_table(std::string(), root, handler);
	}
}


//
int main(int argc, const char *argv[])
{
	for (auto i = 1; i < argc; i++)
	{
		toml2xml(argv[i]);
	}

	return 0;
}