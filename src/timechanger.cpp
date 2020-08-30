#include <string>
#include <cstdio>

#include "ConfigManager.cpp"
#include "SystemTimePort.cpp"

ConfigManager configFile;

int saveRoutine(int argc, char** argv)
{
	std::string helpMessage = "[Timechanger] Save command format: timechanger save <identifier>[ <time formatted in Y-m-d H:i:s.ms>]\n";
	if ( argc < 3 )
	{
		printf(helpMessage.c_str());
		return 1;
	}

	std::string filename(argv[2]);
	filename.append(".tc");

	FILE * f;
	f = fopen(filename.c_str(), "w");

	SystemTimePort st;
	if (argc < 4)
	{
		st.getSystemTime();
	}
	else
	{
		st = std::string(argv[3]);
		if ( st.representation == -1 )
		{
			printf(helpMessage.c_str());
			return 1;
		}
	}

	fprintf(f, st.str().c_str() );

	fclose(f);
	printf("[Timechanger] Time successfully saved to %s\n", filename.c_str());
}

int loadRoutine(int argc, char** argv)
{
	std::string helpMessage = "[Timechanger] Load command format: timechanger load <identifier>\n";
	if ( argc < 3 )
	{
		printf(helpMessage.c_str());
		return 1;
	}

	std::string filename(argv[2]);

	FILE * f;
	f = fopen(filename.c_str(), "r");

	if ( ! f )
	{
		printf("[Timechanger] Unable to find specified file: %s\n", filename.c_str());
		return 1;
	}

	char buffer[256];
	if ( ! fgets(buffer, 255, f) )
	{
		printf("[Timechanger] Failed to load: unknown content\n");
		fclose(f);
		return 1;
	}

	SystemTimePort st;
	st = std::string(buffer);

	fclose(f);
	
	auto keyval = configFile.read();
	uint64_t qwlastLoadTime = 0;
	
	auto it = keyval.find("lastLoadedTime");
	if ( it != keyval.end() )
	{
		qwlastLoadTime = std::stoull(it->second);
		keyval.erase(it);
	}

	SystemTimePort currentSystemTime;
	currentSystemTime.getSystemTime();

	it = keyval.find("pivot");
	if ( it == keyval.end() )
	{
		keyval.insert({"pivot", std::to_string(currentSystemTime.representation)});
	}

	if ( qwlastLoadTime )
	{
		keyval.insert({"delta", std::to_string(currentSystemTime.representation - qwlastLoadTime)});
	}
	
	st.setSystemTime();
	keyval.insert({"lastLoadedTime", std::to_string(st.representation)});
	configFile.write(keyval);

	printf("[Timechanger] Time changed to %s\n", st.str().c_str());
	return 0;
}

int clearRoutine(int argc, char** argv)
{
	configFile.write(ConfigManager::kv_t());
	printf("[Timechanger] Cache cleared\n");
	return 0;
}

int restoreRoutine(int argc, char** argv)
{
	auto kv = configFile.read();
	auto it = kv.find("pivot");
	if ( it == kv.end() )
	{
		printf("[Timechanger] Nothing to restore\n");
		return 1;
	}

	SystemTimePort stp;
	stp.representation = std::stoull(it->second);

	for (auto r = kv.equal_range("delta"); r.first != r.second; r.first++)
	{
		stp.representation += std::stoull(r.first->second);
	}

	stp.setSystemTime();

	printf("[Timechanger] Time restored to %s\n", stp.str().c_str());
	return clearRoutine(argc, argv);
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("[Timechanger] Possible commands are:\n");
		printf("  save\n");
		printf("  load\n");
		printf("  restore\n");
		printf("  clear\n");
		return 1;
	}

	std::string code(argv[1]);
	if 		( code == "save" ) 		return saveRoutine(argc, argv);
	else if ( code == "load" ) 		return loadRoutine(argc, argv);
	else if ( code == "restore" ) 	return restoreRoutine(argc, argv);
	else if ( code == "clear" ) 	return clearRoutine(argc, argv);
	else printf("[Timechanger] Unknown command: %s\n", argv[1]);
	return 0;
}