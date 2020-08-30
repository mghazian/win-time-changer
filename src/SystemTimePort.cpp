#ifndef SystemTimePort_cpp
#define SystemTimePort_cpp

#include <string>
#include <sstream>
#include <vector>
#include <windows.h>
#include <cstdint>

class SystemTimePort
{
	private:
		typedef SYSTEMTIME ST;
	
	protected:
		std::vector <std::string> ssplit (std::string s, const char delim)
		{
			std::vector <std::string> output;
			std::stringstream ss(s);

			for (std::string token; std::getline(ss, token, delim); )
			{
				output.push_back(token);
			}

			return output;
		}

		uint64_t stringToQword (std::string s)
		{
			std::vector <std::string> rawDate = ssplit(std::string(s), ' ');

			if ( rawDate.size() < 2 )
			{
				return -1;
			}

			std::vector <std::string> date = ssplit(rawDate[0], '-');
			for (auto s : ssplit(rawDate[1], ':'))
			{
				date.push_back(s);
			}

			std::string second = date.back();
			date.pop_back();
			for (auto s : ssplit(second, '.'))
			{
				date.push_back(s);
			}

			if ( date.size() != 7 )
			{
				return -1;
			}

			ST st;
			st.wYear 			= std::stoi(date[0]);
			st.wMonth 			= std::stoi(date[1]);
			st.wDay 			= std::stoi(date[2]);
			st.wHour 			= std::stoi(date[3]);
			st.wMinute 			= std::stoi(date[4]);
			st.wSecond 			= std::stoi(date[5]);
			st.wMilliseconds 	= std::stoi(date[6]);

			return this->STToQword(st);
		}

		uint64_t STToQword(ST s)
		{
			FILETIME ft;
			::SystemTimeToFileTime(&s, &ft);
			return (uint64_t) ft.dwLowDateTime | ((uint64_t) ft.dwHighDateTime << 32);
		}

		SYSTEMTIME QwordToST(uint64_t u)
		{
			FILETIME ft;
			SYSTEMTIME st;
			ft.dwLowDateTime = u & 0xFFFFFFFF;
			ft.dwHighDateTime = u >> 32;

			::FileTimeToSystemTime(&ft, &st);
			return st;
		}

	public:
		uint64_t representation;

		SystemTimePort()
		{
			this->representation = 0;
		}

		SystemTimePort(SYSTEMTIME st)
		{
			this->representation = this->STToQword(st);
		}
		
		SystemTimePort(std::string s)
		{
			this->representation = this->stringToQword(s);
		}

		void getSystemTime()
		{
			ST s;
			::GetSystemTime(&s);
			this->representation = this->STToQword(s);
		}

		void setSystemTime()
		{
			SYSTEMTIME st = this->QwordToST(this->representation);
			::SetSystemTime(&st);
		}

		std::string str()
		{
			SYSTEMTIME st = this->QwordToST(this->representation);

			char buffer[512];
			sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
				st.wYear,
				st.wMonth,
				st.wDay,
				st.wHour,
				st.wMinute,
				st.wSecond,
				st.wMilliseconds);
			
			return std::string(buffer);
		}

		SystemTimePort * operator = (std::string s)
		{
			this->representation = this->stringToQword(s);
			return this;
		}

		SystemTimePort * operator = (ST s)
		{
			this->representation = this->STToQword(s);
			return this;
		}

		SystemTimePort operator + (const SystemTimePort & st)
		{
			SystemTimePort output;
			output.representation = this->representation + st.representation;

			return output;
		}

		SystemTimePort operator - (const SystemTimePort & st)
		{
			SystemTimePort output;
			output.representation = this->representation - st.representation;

			return output;
		}
};

#endif