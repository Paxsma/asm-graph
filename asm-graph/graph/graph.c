#include "graph.h"

/* Architectures */
#include "x86/x86.h"


struct graph_structure* graph(void* data, const size_t data_size, const uintptr_t start, const enum cs_arch architecture, const enum cs_mode mode) {


	/* Allocate graph. */
	struct graph_structure* retn = malloc(sizeof(struct graph_structure));

	if (retn == NULL)
		return NULL;


	memset(retn, 0, sizeof(struct graph_structure));


	/* Set settings. */
	retn->arch = architecture;
	retn->mode = mode;
	retn->start_address = start;
	retn->data = data;
	retn->data_size = data_size;


	/* Open capstone handle. */
	csh capstone_handle;
	if (cs_open(architecture, mode, &capstone_handle) != CS_ERR_OK)
		return NULL;
	
	if (cs_option(capstone_handle, CS_OPT_DETAIL, CS_OPT_ON) != CS_ERR_OK)
		return NULL;


	/* Graph by architecture. */
	switch (architecture) {

		case CS_ARCH_X86: {
			graph_x86(retn, capstone_handle);
			break;
		}

	}

	return retn;
}