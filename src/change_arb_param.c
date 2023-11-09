#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vmem_config.h"
#include <vmem/vmem_file.h>
#include <param/param.h>
#include "arb_params.h"

int main(int argc, char **argv){
	int opt;
	int index;
	int is_float = 0;
	int is_int = 1;

	while ((opt = getopt(argc, argv, "fiho:")) != -1) {
		switch (opt) {
			case 'f':
				is_float = 1;
				is_int = 0;
				break;
			case 'i':
				is_int = 1;
				is_float = 0;
				break;
			case 'o':
				index = atoi(optarg);
				break;
			case 'h':
				printf(
						"Usage:\n"
						" <value>	value to be saved\n"
						" -i		use uint16 arb. param\n"
						" -f		use float16 arb. param\n"
						" -o		the index of the param\n"
						);
				return 1;
		};
	};

	vmem_file_init(&vmem_config);

	if (is_int) {
		uint16_t value = (uint16_t) atoi(argv[optind]);
		switch (index) {
			case 0:
				param_set_uint16(&arb_param_int_0, value);
				break;
			case 1:
				param_set_uint16(&arb_param_int_1, value);
				break;
			case 2:
				param_set_uint16(&arb_param_int_2, value);
				break;
			default:
				printf("Index must be within 0-2 range.\n");
				return 1;
		}
	} else {
		float value = (float) atof(argv[optind]);
		switch (index) {
			case 0:
				param_set_float(&arb_param_float_0, value);
				break;
			case 1: 
				param_set_float(&arb_param_float_1, value);
				break;
			default:
				printf("Index must be within 0-1 range.\n");
				return 1;
		}
	}
	return 0;
}
