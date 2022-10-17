#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <capstone/capstone.h>

enum graph_node_type {
	graph_node_type_taken,
	graph_node_type_not_taken,
	graph_node_type_garunteed
};

struct graph_node {

	uintptr_t start;
	uintptr_t end;

	/*
		Node count:
			0: No other nodes.
			1: A garunteed branch.
			2: A comparative branch first node will be branch taken second will be branch not taken.
	*/
	struct graph_node** nodes; 
	uint8_t node_count;

	enum graph_node_type type;
};

struct graph_structure {

	void* data;
	size_t data_size;

	uintptr_t start_address;

	size_t instruction_count;

	cs_arch arch;
	cs_mode mode;

	struct graph_node* main_node;

};

struct graph_structure* graph(void* data, const size_t data_size, const uintptr_t start, const enum cs_arch architecture, const enum cs_mode mode);