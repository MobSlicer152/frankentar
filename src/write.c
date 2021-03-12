#include "frankentar/write.h"

void *ftar_ent_to_raw(struct ftar_ent *ent, size_t *len_ret)
{
	char *buf;
	size_t len;

	errno = 0;

	/* Check arguments */
	if (!ent || !len_ret) {
		errno = EINVAL;
		return NULL;
	}

	/* Figure out how big the buffer should be and allocate it */
	len = (sizeof(struct ftar_ent) - sizeof(char *)) + ent->size;
	buf = calloc(len, 1);
	if (!buf) {
		*len_ret = -1;
		return NULL;
	}

	/* Write in the entry */
	memcpy(buf, ent, sizeof(struct ftar_ent) - sizeof(char *));
	memcpy(buf + (sizeof(struct ftar_ent) - sizeof(char *)), ent->data,
	       ent->size);

	errno = 0;

	/* Return everything */
	*len_ret = len;
	return buf;
}

void *ftar_to_raw(struct ftar *tar, size_t *len_ret)
{
	char *buf;
	char *addr;
	size_t len;
	size_t i;

	errno = 0;

	/* Check arguments */
	if (!tar || !len_ret) {
		errno = EINVAL;
		return NULL;
	}

	/* Figure out how large the buffer should be */
	len = ((sizeof(struct ftar_ent) - sizeof(char *)) * tar->ent_count) +
	      (sizeof(struct ftar) - sizeof(struct ftar_ent **)) +
	      (FTAR_BLOCK_SIZE * 2);
	for (i = 0; i < tar->ent_count; i++)
		len += tar->entries[i]->size;

	/* Allocate the buffer */
	buf = calloc(len, 1);
	if (!buf) {
		*len_ret = -1;
		return NULL;
	}

	/* Copy in the signature and the entries */
	strncpy(buf, FTAR_MAGIC, FTAR_MAGIC_LEN);
	addr = buf + FTAR_MAGIC_LEN;
	memcpy(addr, &tar->ent_count, sizeof(size_t));
	addr += sizeof(size_t);
	for (i = 0; i < tar->ent_count; i++) {
		/* Check the entry's validity */
		if (!tar->entries[i]) {
			errno = EINVAL;
			return NULL;
		}

		/* Copy the entry in */
		memcpy(addr, tar->entries[i],
		       sizeof(struct ftar_ent) - sizeof(char *));
		memcpy(addr + (sizeof(struct ftar_ent) - sizeof(char *)),
		       tar->entries[i]->data, tar->entries[i]->size);

		/* Advance the pointer */
		addr += ((sizeof(struct ftar_ent) - sizeof(char *)) +
			 tar->entries[i]->size);
	}

	/* Even though calloc already does this, clear the last two blocks */
	memset(addr, 0, FTAR_BLOCK_SIZE * 2);

	errno = 0;

	/* Return the buffer */
	*len_ret = len;
	return buf;
}
