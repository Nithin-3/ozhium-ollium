#include "ui/args.h"
#include "invoke.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>

int parseArgs(int argc, char *argv[], ParsedArgs *args) {
	args->element = SLIDER;
	args->min = 0;
	args->max = 100;
	args->current = 50;
	args->action = BACKLIGHT;
	args->text = "";


	// Reset getopt_long internal state for multiple invocations
	optind = 1;

	static struct option longOpts[] = {
		{ "element", required_argument, NULL, 'e' },
		{ "min", required_argument, NULL, 'm' },
		{ "max", required_argument, NULL, 'M' },
		{ "current", required_argument, NULL, 'c' },
		{ "action", required_argument, NULL, 'a' },
		{ "text", required_argument, NULL, 't' },
		{ NULL, 0, NULL, 0 }
	};

	int opt;
	while ((opt = getopt_long(argc, argv, "e:m:M:c:a:t:", longOpts, NULL)) != -1) {
		switch (opt) {
			case 'e': args->element = (GUI_ELEMENT)atoi(optarg); break;
			case 'm': args->min = atof(optarg); break;
			case 'M': args->max = atof(optarg); break;
			case 'c': args->current = atof(optarg); break;
			case 'a': args->action = (SLIDER_ACTION)atoi(optarg); break;
			case 't': args->text = optarg; break;
			default: return 1;
		}
	}
	
	
	return 0;
}
