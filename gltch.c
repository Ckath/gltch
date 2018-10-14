/* see LICENSE file for copyright and license information. */
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

extern int errno;

static void usage();

int 
main(int argc, char *argv[])
{
	if (argc < 2) {
		usage();
	}

	/* initialize with default values */
	unsigned passes = 10;
	unsigned header_size = 20;
	unsigned data_size = 1;
	char *data = malloc(sizeof(char));
	strcpy(data, "\x1");

	
	/* handle options, update values */
	char c;
	while ((c = getopt(argc, argv, "d:n:s:h")) != -1) {
		switch (c) {
			case 'd':
				data_size = strlen(optarg);
				data = realloc(data, sizeof(char)*data_size);
				strcpy(data, optarg);
				break;
			case 'n':
				passes = strtoul(optarg, NULL, 0);
				break;
			case 's':
				header_size = strtoul(optarg, NULL, 0);
				break;
			case 'h':
			default:
				usage();
		}
	}

	FILE *img;
	if (!(img = fopen(argv[argc-1], "r+"))) {
		fprintf(stderr, "gltch: %s: %s\n", argv[argc-1], strerror(errno));
		exit(1);
	}

	struct stat st;
	stat(argv[argc-1], &st);
	unsigned file_size = st.st_size-header_size;

	/* write data to random places within the file */
	srand(getpid());
	unsigned i;
	for (i = 0; i < passes; ++i) {
		fseek(img, (rand()%file_size)+header_size, SEEK_SET);
		fwrite(data, data_size, 1, img);
	}

	fclose(img);
	return 0;
}

static void
usage()
{
	fputs("usage: gltch [-d data] [-n passes] [-s skip/offset] file\n"\
			"-d\tdata to write to random positions, ^A by default\n"\
			"-n\tnumber of passes, 10 by default\n"
			"-s\theaderbytes to skip, 20 by default\n", stderr);
	exit(1);
}
