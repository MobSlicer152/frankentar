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
	char *tar;
	size_t len;
	size_t i;

	/* Allocate a buffer */
	len = FTAR_MAGIC_LEN + (sizeof(struct ftar_ent) - sizeof(char *)) + 5 +
	      FTAR_BLOCK_SIZE * 2;
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
	asdf->mode = FTAR_MODE_USER(FTAR_MODE_FULL) |
		     FTAR_MODE_GROUP(FTAR_MODE_RDWR) |
		     FTAR_MODE_OTHERS(FTAR_MODE_RDWR);
	asdf->size = 5;
	asdf->mtime = time(NULL);

	/* Checksum */
	asdf->checksum = 0;
	for (i = 0; i < strlen(asdf->name); i++)
		asdf->checksum += (unsigned char)asdf->name[i];
	asdf->checksum += asdf->mode + asdf->size + asdf->mtime;
	asdf->checksum += ' ' * 8;

	/* And the rest of the structure (link is already zero) */
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

	/* Print the entries */
	printf("Magic signature: %s\nNumber of entries: %zu\n", ar->magic,
	       ar->ent_count);
	for (i = 0; i < ar->ent_count; i++) {
		printf("\nEntry %zu\nName: %s\nMode: %o\nSize: %zu\n"
		       "Modification time: %lu\nChecksum: %zu\n",
		       i, ar->entries[i]->name, ar->entries[i]->mode,
		       ar->entries[i]->size, ar->entries[i]->mtime,
		       ar->entries[i]->checksum);
		printf("File type: %d\nLink name: %s\nFile contents: ",
		       ar->entries[i]->type, ar->entries[i]->link);
		fwrite(ar->entries[i]->data, ar->entries[i]->size, 1, stdout);
		printf("\n");
	}

	return 0;
}