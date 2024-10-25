#include "../includes/utils.h"
#include <iostream>
int main(int argc, const char **argv)
{
	if (!(argc == 3))
	{
		std::cerr << "Usage: decompress input_filename output_filename\n";
		return 0;
	}
	const char *in_fn = argv[1], *out_fn = argv[2];
	Bitstream bs;
	bs.syncFrom(in_fn);
}
