#define _XOPEN_SOURCE 501

#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

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

#ifdef _MSC_VER
#define S_IFMT _S_IFMT
#define S_IFSOCK _S_IFCHR
#define S_IFLNK _S_IFLNK
#define S_IFREG _S_IFREG
#define S_IFBLK _S_IFCHR
#define S_IFDIR _S_IFDIR
#define S_IFCHR _S_IFCHR
#define S_IFIFO _S_IFIFO
#else
#define S_IFMT __S_IFMT
#define S_IFSOCK __S_IFSOCK
#define S_IFLNK __S_IFLNK
#define S_IFREG __S_IFREG
#define S_IFBLK __S_IFBLK
#define S_IFDIR __S_IFDIR
#define S_IFCHR __S_IFCHR
#define S_IFIFO __S_IFIFO
#endif

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
	FILE *fp;
	size_t len;
	size_t i;
	int err;
#ifdef _MSC_VER
	struct _stat64 st;
#else
	struct stat st;
#endif

	/* Check if we got too few args */
	if (argc < 2)
		ftar_err_exit(EINVAL,
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
	else if (strcmp(argv[1], FTAR_OP_HELP_STR) == 0 ||
		 strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
		op = FTAR_OP_HELP;
	else
		ftar_err_exit(EINVAL,
			      "Error: invalid mode \"%s\" specified,"
			      " see \"%s %s\" for more\n",
			      argv[1], FTAR_GET_BASENAME(argv[0]),
			      FTAR_OP_HELP_STR);

	/* Now do operation specific stuff */
	switch (op) {
	case FTAR_OP_READ:
		/* First, we need more arguments for this, check that */
		if (argc < 3)
			ftar_err_exit(EINVAL,
				      "Error: not enough arguments for "
				      "specified mode, see \"%s %s %s\"\n",
				      FTAR_GET_BASENAME(argv[0]),
				      FTAR_OP_READ_STR, FTAR_OP_HELP_STR);

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
			ftar_err_exit(EINVAL,
				      "Error: not enough arguments for "
				      "specified mode, see \"%s %s %s\"\n",
				      FTAR_GET_BASENAME(argv[0]),
				      FTAR_OP_READ_STR, FTAR_OP_HELP_STR);

		/* Parse our arguments */
		archive = argv[2];
		path = argv[3];

		/* Try to open the file */
		ar = fopen(archive, "rb");
		if (!ar)
			ftar_err_exit(
				errno,
				"Error: failed to open archive \"%s\": %s\n",
				archive, strerror(errno));

		/* Determine its length */
		fseek(ar, 0, SEEK_END);
		len = ftell(ar);
		fseek(ar, 0, SEEK_SET);
		if (!len)
			ftar_err_exit(EINVAL, "Error: empty file\n");

		/* Allocate a buffer */
		ar_cont = calloc(len, sizeof(char));
		if (!ar_cont)
			ftar_err_exit(errno,
				      "Error: failed to allocate buffer: %s\n",
				      strerror(errno));

		/* Read the archive */
		fread(ar_cont, sizeof(char), len, ar);
		if (!ar_cont)
			ftar_err_exit(errno, "Error: failed to read file: %s\n",
				      strerror(errno));

		/* Now parse the archive */
		tar = ftar_load(ar_cont, len);
		if (!tar)
			ftar_err_exit(errno,
				      "Error: failed to parse archive: %s\n",
				      strerror(errno));

		/* Free the buffer */
		free(ar_cont);

		/* Look for the file requested */
		ent = ftar_find(tar, NULL, "%s", path);
		if (!ent)
			ftar_err_exit(errno,
				      "Error: failed to locate file: %s\n",
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
		/* Ensure extra arguments present */
		if (argc < 3)
			ftar_err_exit(EINVAL,
				      "Error: not enough arguments for "
				      "specified mode, see \"%s %s %s\"\n",
				      FTAR_GET_BASENAME(argv[0]),
				      FTAR_OP_CREATE_STR, FTAR_OP_HELP_STR);

		/* Check if help was asked for */
		if (strcmp(argv[2], FTAR_OP_HELP_STR) == 0) {
			printf("Frankentar create mode usage: %s %s <archive"
			       " to create> <one or more files to add>\n",
			       FTAR_GET_BASENAME(argv[0]), FTAR_OP_CREATE_STR);
			return 0;
		}

		/* Check for the rest of our arguments */
		if (argc < 4)
			ftar_err_exit(EINVAL,
				      "Error: not enough arguments for "
				      "specified mode, see \"%s %s %s\"\n",
				      FTAR_GET_BASENAME(argv[0]),
				      FTAR_OP_CREATE_STR, FTAR_OP_HELP_STR);

		/* Initialize our Frankentar structure */
		tar = calloc(1, sizeof(struct ftar));
		if (!tar)
			ftar_err_exit(
				errno,
				"Error: failed to allocate structure: %s\n",
				strerror(errno));
		strncpy(tar->magic, FTAR_MAGIC, FTAR_MAGIC_LEN);

		/* Open the archive */
		if (strcmp(argv[2], "/dev/stdout") !=
			    0 && /* Avoid checking when stdout/stderr is our output */
		    strcmp(argv[2], "/dev/stderr") != 0) {
			ar = fopen(argv[2], "rb");
			if (ar) {
				/* See if we're overwriting something */
				fseek(ar, 0, SEEK_END);
				len = ftell(ar);
				fseek(ar, 0, SEEK_SET);
				if (len) {
					if (ftar_get_y_or_n(
						    "File is not empty. Overwrite it? ")) {
						printf("Overwriting file.\n");

						/* Close, delete, and re-create the file */
						fclose(ar);
						remove(argv[2]);
						ar = fopen(argv[2], "w+b");
						if (!ar)
							ftar_err_exit(
								errno,
								"Error: failed to create file: %s\n",
								strerror(
									errno));
					} else {
						printf("Not overwriting file.\n");
						fclose(ar);
						return ECANCELED;
					}
				}
			} else {
				ar = fopen(argv[2], "w+b");
				if (!ar)
					ftar_err_exit(
						errno,
						"Error: failed to create file: %s\n",
						strerror(errno));
			}
		} else {
			if (strcmp(argv[2], "/dev/stdout") == 0)
				ar = stdout;

			if (strcmp(argv[2], "/dev/stderr") == 0)
				ar = stderr;
		}

		/* Iterate through the remaining arguments */
		for (i = 3; i < argc; i++)
			tar->ent_count++;

		/* Now that we know how many entries we need, allocate them */
		tar->entries =
			calloc(tar->ent_count, sizeof(struct ftar_ent *));
		if (!tar->entries)
			ftar_err_exit(errno,
				      "Error: failed to allocate buffer: %s\n",
				      strerror(errno));

		/* Now go through the entries again and fill out entries */
		for (i = 3; i < argc; i++) {
			/* Stat the file (Windows supports this) */
			err = stat(argv[i], &st);
			if (err < 0)
				ftar_err_exit(
					errno,
					"Error: failed to stat file \"%s\": %s\n",
					strerror(errno));

			/* Allocate the entry */
			tar->entries[i - 3] =
				calloc(1, sizeof(struct ftar_ent));
			if (!tar->entries[i - 3])
				ftar_err_exit(
					errno,
					"Error: failed to allocate structure: %s\n",
					strerror(errno));

			/* Fill in the entry */
			strcpy(tar->entries[i - 3]->name, argv[i]);
			tar->entries[i - 3]->mode =
				(st.st_mode &
				 (FTAR_SET_MODE_USER(FTAR_MODE_FULL) |
				  FTAR_SET_MODE_GROUP(FTAR_MODE_FULL) |
				  FTAR_SET_MODE_OTHERS(FTAR_MODE_FULL)));
			tar->entries[i - 3]->size = st.st_size;
			tar->entries[i - 3]->mtime = st.st_mtime;
			tar->entries[i - 3]->checksum =
				ftar_checksum(tar->entries[i - 3]);

			/* Figure out the file type */
			if (st.st_mode & S_IFREG) {
				tar->entries[i - 3]->type = FTAR_FTYPE_REG;
			} else if (st.st_mode & S_IFLNK) {
				/* Figure out what kind of link this is */
				err = readlink(tar->entries[i - 3]->name,
					       tar->entries[i - 3]->link, 100);
				if (err < 0) { /* Hard link */
					tar->entries[i - 3]->type =
						FTAR_FTYPE_LINK;
					memset(tar->entries[i - 3]->link, 0,
					       100);
				} else {
					tar->entries[i - 3]->type =
						FTAR_FTYPE_SYMLINK;
				}
			} else if (st.st_mode & S_IFSOCK ||
				   st.st_mode & S_IFCHR ||
				   st.st_mode & S_IFBLK) {
				tar->entries[i - 3]->type = FTAR_FTYPE_SPECIAL;
			} else if (st.st_mode & S_IFDIR) {
				tar->entries[i - 3]->type = FTAR_FTYPE_DIR;
			} else if (st.st_mode & S_IFIFO) {
				tar->entries[i - 3]->type = FTAR_FTYPE_FIFO;
			}

			/* Allocate room for the file's contents */
			tar->entries[i - 3]->data =
				calloc(tar->entries[i - 3]->size, sizeof(char));
			if (!tar->entries[i - 3]->data)
				ftar_err_exit(
					errno,
					"Error: failed to allocate buffer: %s\n",
					strerror(errno));

			/* Open the file */
			fp = fopen(argv[i], "rb");
			if (!fp)
				ftar_err_exit(
					errno,
					"Error: failed to open file: %s\n",
					strerror(errno));

			/* Read the file */
			err = fread(tar->entries[i - 3]->data, sizeof(char),
				    tar->entries[i - 3]->size, fp);
			if (err < 0)
				ftar_err_exit(errno,
					      "Error: failed to read file %s\n",
					      strerror(errno));

			/* Close the file */
			fclose(fp);
		}

		/* Convert the tar into a buffer */
		ar_cont = ftar_to_raw(tar, &len);
		if (!ar_cont)
			ftar_err_exit(
				errno,
				"Error: failed to convert to buffer: %s\n",
				strerror(errno));

		/* Free the original structure */
		ftar_free(tar);

		/* Write the buffer */
		err = fwrite(ar_cont, sizeof(char), len, ar);
		if (!err)
			ftar_err_exit(
				errno,
				"Error: failed to write archive to file: %s\n",
				strerror(errno));

		/* Free the buffer and close the file */
		free(ar_cont);
		fclose(ar);

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
