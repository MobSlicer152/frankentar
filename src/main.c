#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "frankentar.h"
#include "frankentar/read.h"
#include "frankentar/util.h"
#include "frankentar/write.h"

#define FTAR_OP_READ_STR "read"
#define FTAR_OP_LIST_STR "list"
#define FTAR_OP_FIND_STR "find"
#define FTAR_OP_CREATE_STR "create"
#define FTAR_OP_ADD_STR "add"
#define FTAR_OP_DEL_STR "delete"

#define FTAR_OP_READ 0
#define FTAR_OP_LIST 1
#define FTAR_OP_FIND 2
#define FTAR_OP_CREATE 3
#define FTAR_OP_ADD 4
#define FTAR_OP_DEL 5

int main(int argc, char *argv[])
{
	/* General variables that are used all over */
	char op;
	char *archive;
	char *path; 
	struct ftar *tar;
	struct ftar_ent *ent;

	/* Check if we got too few args */
	if (argc < 2)
		err_exit(EINVAL, "Error: not enough arguments\n");

	/* Now figure out what mode we're in */
	if (strcmp(argv[1], FTAR_OP_READ_STR) == 0)
		op = FTAR_OP_READ;
	else if (strcmp(argv[1], FTAR_OP_LIST_STR) == 0)
		op = FTAR_OP_LIST;
	else if (strcmp(argv[1], FTAR_OP_FIND_STR) == 0)
		op = FTAR_OP_FIND;
	else if (strcmp(argv[1], FTAR_OP_CREATE_STR) == 0)
		op = FTAR_OP_CREATE;
	else if (strcmp(argv[1], FTAR_OP_ADD_STR) == 0)
		op = FTAR_OP_ADD;
	else if (strcmp(argv[1], FTAR_OP_DEL_STR) == 0)
		op = FTAR_OP_DEL;
	else
		err_exit(EINVAL, "Error: invalid mode \"%s\" specified\n", argv[1]);

	/* Now do operation specific stuff */
	switch (op) {
	case FTAR_OP_READ:
		/* First, we need more arguments for this, check that */
		if (argc < 4)
			err_exit(EINVAL, "Error: not enough arguments for "
				 "specified mode\n");

		/*  */

		break;
	}

	return 0;
}
