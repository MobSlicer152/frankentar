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
#define FTAR_OP_CREATE_STR "create"
#define FTAR_OP_ADD_STR "add"

#define FTAR_OP_READ 0
#define FTAR_OP_LIST 1
#define FTAR_OP_FIND 2
#define FTAR_OP_CREATE 3
#define FTAR_OP_ADD 4

int main(int argc, char *argv[])
{
	
}