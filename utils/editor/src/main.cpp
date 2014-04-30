#include "EdApplication.hpp"
#include "TileSettings.hpp"

int main(int argc, char *argv[])
{
	return EdApplication::getInstance().execute(argc, argv);;
}
