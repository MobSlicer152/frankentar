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
#define FTAR_OP_EXTR_STR "extract"
#define FTAR_OP_HELP_STR "help"

#define FTAR_OP_READ 0
#define FTAR_OP_LIST 1
#define FTAR_OP_FIND 2
#define FTAR_OP_CREATE 3
#define FTAR_OP_ADD 4
#define FTAR_OP_DEL 5
#define FTAR_OP_EXTR 6
#define FTAR_OP_HELP 7

int main(int argc, char *argv[])
{
	/* General variables that are used all over */
	char op;
	char *archive;
	void *ar_cont;
	char *path;
	struct ftar *tar;
	struct ftar_ent *ent;
	FILE *ar;
	size_t len;

	/* Check if we got too few args */
	if (argc < 2)
		err_exit(EINVAL,
			 "Error: not enough arguments, see \"%s %s\" for"
			 " more information\n",
			 FTAR_GET_BASENAME(argv[0]), FTAR_OP_HELP_STR);

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
	else if (strcmp(argv[1], FTAR_OP_EXTR_STR) == 0)
		op = FTAR_OP_EXTR;
	else if (strcmp(argv[1], FTAR_OP_HELP_STR) == 0)
		op = FTAR_OP_HELP;
	else
		err_exit(EINVAL,
			 "Error: invalid mode \"%s\" specified,"
			 " see \"%s %s\" for more\n",
			 argv[1], FTAR_GET_BASENAME(argv[0]), FTAR_OP_HELP_STR);

	/* Now do operation specific stuff */
	switch (op) {
	case FTAR_OP_READ:
		/* First, we need more arguments for this, check that */
		if (argc < 3)
			err_exit(EINVAL,
				 "Error: not enough arguments for "
				 "specified mode, see \"%s %s %s\"\n",
				 FTAR_GET_BASENAME(argv[0]), FTAR_OP_READ_STR,
				 FTAR_OP_HELP_STR);

		/* Check if help was requested */
		if (strcmp(argv[2], FTAR_OP_HELP_STR) == 0) {
			printf("Frankentar %s mode usage: %s %s <archive> "
			       "<file to read>\n",
			       FTAR_OP_READ_STR, FTAR_GET_BASENAME(argv[0]),
			       FTAR_OP_READ_STR);
			return 0;
		}

		/* Now, check if we got enough arguments to *do* something */
		if (argc < 4)
			err_exit(EINVAL,
				 "Error: not enough arguments for "
				 "specified mode, see \"%s %s %s\"\n",
				 FTAR_GET_BASENAME(argv[0]), FTAR_OP_READ_STR,
				 FTAR_OP_HELP_STR);

		/* Parse our arguments */
		archive = argv[2];
		path = argv[3];

		/* Try to open the file */
		ar = fopen(archive, "rb");
		if (!ar)
			err_exit(errno,
				 "Error: failed to open archive \"%s\": %s\n",
				 archive, strerror(errno));

		/* Determine its length */
		fseek(ar, 0, SEEK_END);
		len = ftell(ar);
		fseek(ar, 0, SEEK_SET);
		if (!len)
			err_exit(EINVAL, "Error: empty file\n");

		/* Allocate a buffer */
		ar_cont = calloc(len, sizeof(char));
		if (!ar_cont)
			err_exit(errno,
				 "Error: failed to allocate buffer: %s\n",
				 strerror(errno));

		/* Read the archive */
		fread(ar_cont, sizeof(char), len, ar);
		if (!ar_cont)
			err_exit(errno, "Error: failed to read file: %s\n",
				 strerror(errno));

		/* Now parse the archive */
		tar = ftar_load(ar_cont, len);
		if (!tar)
			err_exit(errno, "Error: failed to parse archive: %s\n",
				 strerror(errno));

		/* Free the buffer */
		free(ar_cont);

		/* Look for the file requested */
		ent = ftar_find(tar, NULL, "%s", path);
		if (!ent)
			err_exit(errno, "Error: failed to locate file: %s\n",
				 strerror(errno));

		/* Print the entry */
		ftar_print_ent(ent);

		/*
		 * Free the archive details (individual entries can be freed
		 *  normally, this frees all the ones in the archive structure)
		 */
		ftar_free(tar);

		break;
	case FTAR_OP_CREATE:
		size_t nfiles;

		/* Ensure extra arguments present */
		if (argc < 3)
			err_exit(EINVAL,
				 "Error: not enough arguments for "
				 "specified mode, see \"%s %s %s\"\n",
				 FTAR_GET_BASENAME(argv[0]), FTAR_OP_CREATE_STR,
				 FTAR_OP_HELP_STR);

		/* Check if help was asked for */
		if (strcmp(argv[2], FTAR_OP_HELP_STR) == 0)
			printf("Frankentar create mode usage: %s %s <archive"
			       " to create> <one or more files to add>\n",
			       FTAR_GET_BASENAME(argv[0]), FTAR_OP_CREATE_STR);

		break;
	case FTAR_OP_HELP:
	default:
		/* Print a help message */
		printf("Usage: %s <mode> <additional args> ...\n"
		       "Where \"mode\" is one of the following:\n"
		       "  read - read the details of a file from the archive\n"
		       "  list - list the files in the archive\n"
		       "  find - determine whether a file is in the archive\n"
		       "  create - create an archive with the given files\n"
		       "  add - add a file to the archive, overwriting any"
		       " file with the same name\n"
		       "  delete - delete a file from the archive\n"
		       "  extract - extract all or specified files from the"
		       " archive\n"
		       "  help - print this help message\n\n"
		       "Arguments in angle brackets (<>) are mandatory, while"
		       " those in square brackets ([]) are optional.\n",
		       FTAR_GET_BASENAME(argv[0]));

		break;
	}

	return 0;
}
