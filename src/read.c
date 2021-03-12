#include "frankentar/read.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ftar *ftar_load(void *tar, size_t tar_len)
{
	struct ftar *new;
	struct ftar_ent *ent;
	char *addr;
	char *t;
	size_t i;

	/* Check our arguments */
	if (!tar || !tar_len) {
		errno = EINVAL;
		return NULL;
	}

	/* Make a local copy of tar */
	t = calloc(tar_len, 1);
	if (!t)
		return NULL;
	memcpy(t, tar, tar_len);

	/* Allocate the structure */
	new = calloc(1, sizeof(struct ftar));
	if (!new)
		return NULL;

	/* Validate the file signature */
	memcpy(new->magic, t, FTAR_MAGIC_LEN);
	for (i = 0; i < FTAR_MAGIC_LEN; i++) {
		if (new->magic[i] != FTAR_MAGIC[i]) {
			errno = EINVAL;
			return NULL;
		}
	}

	/* Get the number of entries */
	addr = t + FTAR_MAGIC_LEN;
	memcpy(&new->ent_count, addr, sizeof(size_t));
	if (!new->ent_count) {
		errno = EINVAL;
		return NULL;
	}

	/* Allocate the entries */
	new->entries = calloc(new->ent_count, sizeof(struct ftar_ent *));
	if (!new->entries)
		return NULL;

	/* Read each entry into its structure (yay pointer arithmetic!) */
	addr += sizeof(size_t);
	for (i = 0; i < new->ent_count; i++) {
		/* Read this header */
		new->entries[i] = calloc(1, sizeof(struct ftar_ent));
		if (!new->entries[i])
			return NULL;
		ent = new->entries[i];
		memcpy(ent->name, addr,
		       (sizeof(*new->entries[i]) - sizeof(char *)));

		/* Read the file for this entry */
		ent->data =
			calloc(ent->size, sizeof(char));
		if (!ent->data)
			return NULL;
		memcpy(ent->data,
		       addr + (sizeof(*ent) - sizeof(char *)),
		       ent->size);

		/* Jump to the next entry */
		addr += ((ent->size / FTAR_BLOCK_SIZE) + 1) *
			FTAR_BLOCK_SIZE;
		if (ent->size % FTAR_BLOCK_SIZE)
			addr += FTAR_BLOCK_SIZE;
	}

	/* Now we're done */
	return new;
}

struct ftar_ent *ftar_find(struct ftar *tar, long *index, const char *name, ...)
{
	size_t name_len;
	char *name_fmt;
	struct ftar_ent *ent;
	size_t i;
	va_list args;

	errno = 0;

	/* Check parameters */
	if (!tar || !name) {
		errno = EINVAL;
		return NULL;
	}

	/* Format name */
	va_start(args, name);
	name_fmt = ftar_fmt_text_va(&name_len, name, args);
	va_end(args);

	/* Loop through entries */
	for (i = 0; i < tar->ent_count; i++) {
		/* Check if the name matches */
		if (strcmp(tar->entries[i]->name, name_fmt) == 0) {
			ent = tar->entries[i];
			break;
		}
	}

	/* Check if we failed to find name in the archive */
	if (!ent) {
		errno = ENOENT;
		if (index)
			*index = -1;
		return NULL;
	}

	/* Free name_fmt */
	(name_len) ? (void)0 : free(name_fmt);

	errno = 0;

	/* Return ent, and if it's requested, index too */
	if (index)
		*index = i;
	return ent;
}

long ftar_checksum(struct ftar_ent *ent)
{
	long ret;
	size_t i;

	/* Validate our parameter */
	if (!ent) {
		errno = EINVAL;
		return -1;
	}

	/* Calculate the sum */
	ret = 0;
	for (i = 0; i < strlen(ent->name); i++)
		ret += ((unsigned char *)ent->name)[i];
	ret += (ent->mode + ent->size + ent->mtime);
	ret += ' ' * 8; /*
			   * According to the tar spec, ent->checksum should be
			   *  treated as eight spaces for this calculation
			   */

	/* Check if we're validating the checksum or just calculating it */
	if (ent->checksum)
		ret = (ret == ent->checksum) ? 1 : 0;
	else
		ent->checksum = ret;

	return ret;
}

void ftar_print_ent(struct ftar_ent *ent)
{
	struct tm *now;

	errno = 0;

	/* Check argument */
	if (!ent) {
		errno = EINVAL;
		return;
	}

	/* Turn the modification time of the entry into a time structure */
	now = localtime(&ent->mtime);

	/* Print our entry */
	printf("Name: %s\nMode: user %o, group %o, others %o\nSize: %zu\n"
	       "Modification time: %d:%d:%d %d/%d/%d (%lu)\nChecksum: %zu\n",
	       ent->name, FTAR_GET_MODE_USER(ent->mode),
	       FTAR_GET_MODE_GROUP(ent->mode), FTAR_GET_MODE_OTHERS(ent->mode),
	       ent->size, now->tm_hour, now->tm_min, now->tm_sec, now->tm_mday,
	       now->tm_mon + 1, now->tm_year + 1900, ent->mtime, ent->checksum);
	printf("File type: %d\nLink name: %s\nFile contents: ", ent->type,
	       ent->link);
	fwrite(ent->data, ent->size, 1, stdout);

	/* If necessary, write a newline */
	if (ent->data[ent->size - 1] != '\n')
		printf("\n");

	errno = 0;
}

void ftar_free(struct ftar *tar)
{
	size_t i;

	errno = 0;

	/* Avoid a segfault */
	if (!tar) {
		errno = EINVAL;
		return;
	}

	/* Free the entries */
	for (i = 0; i < tar->ent_count; i++) {
		if (tar->entries[i])
			free(tar->entries[i]);
	}

	/* Free the structure */
	free(tar);

	errno = 0;
}

#ifdef __cplusplus
}
#endif
