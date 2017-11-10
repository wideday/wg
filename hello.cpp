
#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
	std::cout << "Hello, world!" << std::endl;
	if (!std::cout.good())
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}
