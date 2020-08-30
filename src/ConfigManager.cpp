#ifndef ConfigManager_cpp
#define ConfigManager_cpp

#include <string>
#include <cstdio>
#include <unordered_map>

class ConfigManager
{
	private:
		const std::string filename = "tc.conf";

	public:
		typedef std::unordered_multimap <std::string, std::string> kv_t;
		kv_t keyval;

		void parseKeyval(std::string s, char separator = '=')
		{
			int separatorIndex = s.find(separator);
			std::string k = s.substr(0, separatorIndex);
			std::string v = s.substr(std::min((int) s.size(), separatorIndex + 1));

			if ( k == "" )
			{
				return;
			}

			(this->keyval).insert({k, v});
		};
		
		kv_t read()
		{
			FILE * f = fopen((this->filename).c_str(), "r");
			if ( ! f )
			{
				return kv_t ();
			}

			this->keyval = kv_t();

			char buffer[1024];
			while ( fgets(buffer, 1024, f) )
			{
				std::string line(buffer);
				
				int index;
				while ((index = line.find("\n")) != std::string::npos)
				{
					line.erase(index, 1);
				}
				
				parseKeyval(line);
			}

			fclose(f);

			return this->keyval;
		}

		bool write(kv_t kv)
		{
			FILE * f = fopen((this->filename).c_str(), "w");
			if ( ! f )
			{
				return false;
			}

			for (auto e : kv)
			{
				fprintf(f, "%s=%s\n", e.first.c_str(), e.second.c_str());
			}

			fclose(f);
			return true;
		}
};

#endif