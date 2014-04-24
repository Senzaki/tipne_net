#include "EdApplication.hpp"

int main(int argc, char *argv[])
{
	return EdApplication::getInstance().execute(argc, argv);;
}
