#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "frankentar.h"
#include "frankentar/read.h"
#include "frankentar/write.h"

#define FTAR_OP_READ_STR "read"
#define FTAR_OP_LIST_STR "list"
#define FTAR_OP_FIND_STR "find"
#define FTAR_OP_READ 0

int main(int argc, char *argv[])
{
	struct ftar *ar;
	struct ftar_ent *asdf;
	struct ftar_ent *found;
	char *tar;
	size_t len;
	size_t i;
	long idx;
	struct tm *now;

	/* Allocate a buffer */
	len = FTAR_MAGIC_LEN + (sizeof(struct ftar_ent) - sizeof(char *)) + 5 + /* 5 is to be the length of the file */
	      FTAR_BLOCK_SIZE * 2; /* The last two 512-byte blocks have to be NULL */
	tar = calloc(len, sizeof(char));
	if (!tar) {
		fprintf(stderr, "Error: failed to allocate buffer: %s\n",
			strerror(errno));
		return errno;
	}

	/* Allocate a Frankentar entry in memory */
	asdf = calloc(1, sizeof(struct ftar_ent));
	if (!asdf) {
		fprintf(stderr, "Error: failed to allocate structure: %s\n",
			strerror(errno));
		return errno;
	}

	/* Fill in the entry */
	sprintf(asdf->name, "test.txt"); /* The file will be called test.txt */
	asdf->mode = FTAR_SET_MODE_USER(FTAR_MODE_FULL) |
		     FTAR_SET_MODE_GROUP(FTAR_MODE_RDWR) |
		     FTAR_SET_MODE_OTHERS(FTAR_MODE_RDWR);
	asdf->size = 5;
	asdf->mtime = time(NULL);
	asdf->checksum = ftar_checksum(asdf);
	asdf->type = 0; /* Regular file */
	asdf->data = calloc(asdf->size, sizeof(char));
	if (!asdf->data) {
		fprintf(stderr, "Error: failed to allocate buffer: %s\n",
			strerror(errno));
		return errno;
	}
	strncpy(asdf->data, "asdf\n", asdf->size);

	/* Write the magic string and the entry (calloc zeroes the NULL ones) */
	strncpy(tar, FTAR_MAGIC, FTAR_MAGIC_LEN);
	memcpy((tar + FTAR_MAGIC_LEN), asdf,
	       sizeof(struct ftar_ent) - sizeof(char *));
	memcpy(tar + FTAR_MAGIC_LEN +
		       (sizeof(struct ftar_ent) - sizeof(char *)),
	       asdf->data, asdf->size);

	/* See if ftar_load sees our buffer as valid */
	ar = ftar_load(tar, len);
	if (!ar) {
		fprintf(stderr,
			"Error: failed to load Frankentar archive: %s\n",
			strerror(errno));
		return errno;
	}

	/* Find our entry */
	found = ftar_find(ar, &idx, "test.txt");
	if (!found) {
		fprintf(stderr, "Error: couldn't find entry: %s\n", strerror(errno));
		return errno;
	}

	/* Turn the modification time of the entry into a string */
	now = localtime(&asdf->mtime);

	/* Print the entry we found */
	printf("Index of found entry: %zu\nName: %s\nMode: %o\nSize: %zu\n"
	       "Modification time: %d:%d:%d %d/%d/%d (%lu)\nChecksum: %zu\n",
	       idx, ar->entries[idx]->name, ar->entries[idx]->mode,
	       ar->entries[idx]->size, now->tm_hour, now->tm_min,
	       now->tm_sec, now->tm_mday, now->tm_mon + 1, now->tm_year - 70,
	       ar->entries[idx]->mtime, ar->entries[idx]->checksum);
	printf("File type: %d\nLink name: %s\nFile contents: ",
	       ar->entries[idx]->type, ar->entries[idx]->link);
	fwrite(ar->entries[idx]->data, ar->entries[idx]->size, 1, stdout);
	
	/* If necessary, write a newline */
	if (ar->entries[idx]->data[ar->entries[idx]->size - 1] != '\n')
		printf("\n");

	/* Free the ~240/entry (sorry!) bytes used by the structure */
	ftar_free(ar);

	return 0;
}