#include <capstone/x86.h>
#include "x86.h"

void graph_x86(struct graph_structure* graph, const csh capstone_handle) {

	cs_insn* capstone_disasm;


	uintptr_t pc = 0u;
	const uintptr_t graph_data = (const uintptr_t)graph->data;
	const uintptr_t graph_max_addr = graph->start_address + graph->data_size;



	graph->main_node = malloc(sizeof(struct graph_node));
	if (graph->main_node == NULL)
		return;

	memset(graph->main_node, 0, sizeof(struct graph_node));



	/* Nodes to search through for certain nodes. */
	size_t node_count = 0u;
	struct graph_node** nodes = malloc(sizeof(struct graph_node*) * 3u);
	if (nodes == NULL)
		return;

	memset(nodes, 0, sizeof(struct graph_node*) * 3u);



	nodes[node_count++] = graph->main_node; /* Always furthest branch. */


	do {

		const size_t count = cs_disasm(capstone_handle, (const uint8_t*)(graph_data + pc), graph->data_size, 1u, pc, &capstone_disasm);
		
		for (size_t i = 0u; i < count; ++i) {

			const cs_insn data = capstone_disasm[i];

			switch (data.id) {

				case X86_INS_JMP: {

					/* Not constant jump. */
					if (data.detail->x86.operands[0].type != X86_OP_IMM)
						break;


					const int64_t jump = data.detail->x86.operands[0].imm;
					const int64_t rel_jump = jump - (pc + graph->start_address);
					if (jump > (signed)graph_max_addr || (signed)graph->start_address > jump) /* Jump is greater or less then dissassembly size ignore it. */
						break;




					/* Allocate branches. */
					struct graph_node* branch = malloc(sizeof(struct graph_node));
					if (branch == NULL)
						return;

					memset(branch, 0, sizeof(struct graph_node));
					branch->type = graph_node_type_taken;




					/* Jump back. */
					if (rel_jump < 0) {

						/* Were gonna replace it with another node so just free it. */
						free(branch);

						branch = NULL;

						/* Find greatest start thats not bigger then jump. */
						for (size_t n = 0u; n < node_count; ++n)
							if (nodes[n]->start < (signed)jump && (branch == NULL || branch->start > nodes[n]->start))
								branch = nodes[n];

					}


					/* Get current node by greatest start but not bigger then jump. */
					struct graph_node* curr_node = NULL;
					for (size_t n = 0u; n < node_count; ++n) {

						if (nodes[n]->start < (signed)jump && (curr_node == NULL || curr_node->start > nodes[n]->start))
							curr_node = nodes[n];

					}


					/* Log all nodes that have the same start. */
					for (size_t n = 0u; n < node_count; ++n)
						if (nodes[n] != NULL && nodes[n]->start == curr_node->start) {

							/* Log branches. */
							nodes[node_count++] = branch;

							/* Start and end for current. */
							branch->start = pc + data.size;

							nodes[n]->end = graph->start_address + pc;


							/* Set nodes. */
							nodes[n]->nodes = malloc(sizeof(struct graph_node*) * 2u);
							if (nodes[n]->nodes == NULL)
								return;

							memset(nodes[n]->nodes, 0, sizeof(struct graph_node*) * 2u);


							nodes[n]->nodes[0] = branch;
							nodes[n]->node_count = 1u;

							/* Allocate more room for new branches. */
							nodes = realloc(nodes, (sizeof(struct graph_node*) * node_count) + (sizeof(struct graph_node*) * 2u));
							if (nodes == NULL)
								return;

							memset(&nodes[node_count], 0, (sizeof(struct graph_node*) * 2u));

						}


					break;
				}

				case X86_INS_JAE:
				case X86_INS_JA:
				case X86_INS_JBE:
				case X86_INS_JB:
				case X86_INS_JCXZ:
				case X86_INS_JECXZ:
				case X86_INS_JE:
				case X86_INS_JGE:
				case X86_INS_JG:
				case X86_INS_JLE:
				case X86_INS_JL:
				case X86_INS_JNE:
				case X86_INS_JNO:
				case X86_INS_JNP:
				case X86_INS_JNS:
				case X86_INS_JO:
				case X86_INS_JP:
				case X86_INS_JRCXZ:
				case X86_INS_JS: {


					/* Not constant jump. */
					if (data.detail->x86.operands[0].type != X86_OP_IMM)
						break;


					const int64_t jump = data.detail->x86.operands[0].imm;
					const int64_t rel_jump = jump - (pc + graph->start_address);
					if (jump > (signed)graph_max_addr || (signed)graph->start_address > jump) /* Jump is greater or less then dissassembly size ignore it. */
						break;




					/* Allocate branches. */
					struct graph_node* node_branch_taken = malloc(sizeof(struct graph_node));
					if (node_branch_taken == NULL)
						return;

					memset(node_branch_taken, 0, sizeof(struct graph_node));
					node_branch_taken->type = graph_node_type_taken;


					struct graph_node* node_branch_not_taken = malloc(sizeof(struct graph_node));
					if (node_branch_not_taken == NULL)
						return;

					memset(node_branch_not_taken, 0, sizeof(struct graph_node));
					node_branch_not_taken->type = graph_node_type_not_taken;




					/* Jump back. */
					if (rel_jump < 0) {

						/* Were gonna replace it with another node so just free it. */
						free(node_branch_taken);

						node_branch_taken = NULL;

						/* Find greatest start thats not bigger then jump. */
						for (size_t n = 0u; n < node_count; ++n)
							if (nodes[n]->start < (signed)jump && (node_branch_taken == NULL || node_branch_taken->start > nodes[n]->start))
								node_branch_taken = nodes[n];

					}


					/* Get current node by greatest start but not bigger then jump. */
					struct graph_node* curr_node = NULL;
					for (size_t n = 0u; n < node_count; ++n) {

						if (nodes[n]->start < (signed)jump && (curr_node == NULL || curr_node->start > nodes[n]->start))
							curr_node = nodes[n];
	
					}


					/* Log all nodes that have the same start. */
					for (size_t n = 0u; n < node_count; ++n)
						if (nodes[n] != NULL && nodes[n]->start == curr_node->start) {
						
							/* Log branches. */
							nodes[node_count++] = node_branch_taken;
							nodes[node_count++] = node_branch_not_taken;
	

							/* Start and end for current. */
							node_branch_not_taken->start = pc + data.size;
							node_branch_taken->start = data.detail->x86.operands[0].imm;

							nodes[n]->end = graph->start_address + pc;

							
							/* Set nodes. */
							nodes[n]->nodes = malloc(sizeof(struct graph_node*) * 2u);
							if (nodes[n]->nodes == NULL)
								return;

							memset(nodes[n]->nodes, 0, sizeof(struct graph_node*) * 2u);
	

							nodes[n]->nodes[0] = node_branch_taken;
							nodes[n]->nodes[1] = node_branch_not_taken;
							nodes[n]->node_count = 2u;
						

							/* Allocate more room for new branches. */
							nodes = realloc(nodes, (sizeof(struct graph_node*) * node_count) + (sizeof(struct graph_node*) * 2u));
							if (nodes == NULL)
								return;
							
							memset(&nodes[node_count], 0, (sizeof(struct graph_node*) * 2u));
						
						}

					break;
				}

			}

			pc += data.size;

		}

		cs_free(capstone_disasm, count);

	} while (pc < graph->data_size);


	free(nodes);
	return;
}