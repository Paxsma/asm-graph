#include "graph/graph.h"

int32_t main() {


	uint8_t data[] = {
		0x74 ,0x10,          // 00000000        je     12 <__A>
		0x89 ,0xc0,          // 00000002        mov    eax,eax
		0x70 ,0x12,          // 00000004        jo     18 <__A_1>
		0x70 ,0x10,          // 00000006        jo     18 <__A_1>
		0x70 ,0x0e,          // 00000008        jo     18 <__A_1>
		0x70 ,0x0c,          // 00000010        jo     18 <__A_1>
		0x89 ,0xc0,          // 00000012        mov    eax,eax
		0x70 ,0x08,          // 00000014        jo     18 <__A_1>
		0x70 ,0x06,          // 00000016        jo     18 <__A_1>   
		// 00000018 <__A>:
		0x70 ,0x04,          // 00000020        jo     18 <__A_1>
		0x89 ,0xc0,          // 00000022        mov    eax,eax
		0x31 ,0xc0,          // 00000024        xor    eax,eax 
		// 00000025 <__A_1>:
		0x31 ,0xc0          //  00000028        xor    eax,eax
	};

	const struct graph_structure* graph_struct = graph(data, sizeof(data), 0u, CS_ARCH_X86, CS_MODE_32);

	/* Just prints out first branch. */
	struct graph_node* analzing = NULL;
	do {

		/* Get first node. */
		if (analzing == NULL && graph_struct->main_node != NULL)
			analzing = graph_struct->main_node;
		else if (analzing != NULL && analzing->node_count)
			analzing = analzing->nodes[0];

		printf("[Node] start: %llx  end: %llx \n", analzing->start, analzing->end);

	} while (analzing == NULL || analzing->node_count);


	return 0;
}