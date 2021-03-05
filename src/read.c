#include "frankentar/read.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ftar *ftar_load(void *tar, size_t tar_len)
{
	struct ftar *new;
	void *addr;
	struct ftar_ent *ent;
	void *t;
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

	/* Count the entries */
	addr = t + FTAR_MAGIC_LEN;
	for (i = 0;; i++) {
		/* Read this header */
		ent = addr;
		if (!ent->name[0])
			break;

		/* Get the address of the next entry */
		addr += ((ent->size / FTAR_BLOCK_SIZE) + 1) * FTAR_BLOCK_SIZE;
		if (ent->size % FTAR_BLOCK_SIZE)
			addr += FTAR_BLOCK_SIZE;
	}
	new->ent_count = i;

	/* Allocate the entries */
	new->entries = calloc(new->ent_count, sizeof(struct ftar_ent *));
	if (!new->entries)
		return NULL;

	/* Read each entry into its structure (yay pointer arithmetic!) */
	addr = t + FTAR_MAGIC_LEN;
	for (i = 0; i < new->ent_count; i++) {
		/* Read this header */
		new->entries[i] = calloc(1, sizeof(struct ftar_ent));
		if (!new->entries[i])
			return NULL;
		memcpy(new->entries[i]->name, addr,
		       (sizeof(*new->entries[i]) - sizeof(char *)));

		/* Read the file for this entry */
		new->entries[i]->data =
			calloc(new->entries[i]->size, sizeof(char));
		if (!new->entries[i]->data)
			return NULL;
		memcpy(new->entries[i]->data,
		       addr + (sizeof(*new->entries[i]) - sizeof(char *)),
		       new->entries[i]->size);

		/* Jump to the next entry */
		addr += ((new->entries[i]->size / FTAR_BLOCK_SIZE) + 1) *
			FTAR_BLOCK_SIZE;
		if (new->entries[i]->size % FTAR_BLOCK_SIZE)
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
	name_fmt = fmt_text_va(&name_len, name, args);
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

#ifdef __cplusplus
}
#endif
