#include "Application.hpp"
#include "PureServerApplication.hpp"
#include <cstring>

int main(int argc, char **argv)
{
	for(int i = 1; i < argc; i++)
	{
		if(std::strcmp(argv[i], "--pure-server") == 0)
			return PureServerApplication::getInstance().execute(argc, argv);
	}

	return Application::getInstance().execute(argc, argv);
}
