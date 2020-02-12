#ifndef BABELTRACE_PLUGIN_TEXT_PRETTY_PRETTY_H
#define BABELTRACE_PLUGIN_TEXT_PRETTY_PRETTY_H

/*
 * Copyright 2016 Jérémie Galarneau <jeremie.galarneau@efficios.com>
 *
 * Author: Jérémie Galarneau <jeremie.galarneau@efficios.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <glib.h>
#include <stdio.h>
#include <stdbool.h>
#include "common/macros.h"
#include <babeltrace2/babeltrace.h>

enum pretty_default {
	PRETTY_DEFAULT_UNSET,
	PRETTY_DEFAULT_SHOW,
	PRETTY_DEFAULT_HIDE,
};

enum pretty_color_option {
	PRETTY_COLOR_OPT_NEVER,
	PRETTY_COLOR_OPT_AUTO,
	PRETTY_COLOR_OPT_ALWAYS,
};

struct pretty_options {
	char *output_path;

	enum pretty_default name_default;
	enum pretty_default field_default;

	bool print_scope_field_names;
	bool print_header_field_names;
	bool print_context_field_names;
	bool print_payload_field_names;

	bool print_delta_field;
	bool print_loglevel_field;
	bool print_emf_field;
	bool print_callsite_field;
	bool print_trace_field;
	bool print_trace_domain_field;
	bool print_trace_procname_field;
	bool print_trace_vpid_field;
	bool print_trace_hostname_field;

	bool print_timestamp_cycles;
	bool clock_seconds;
	bool clock_date;
	bool clock_gmt;
	enum pretty_color_option color;
	bool verbose;
};

struct pretty_component {
	struct pretty_options options;
	bt_message_iterator *iterator;
	FILE *out, *err;
	int depth;	/* nesting, used for tabulation alignment. */
	bool start_line;
	GString *string;
	GString *tmp_string;
	bool use_colors;

	uint64_t last_cycles_timestamp;
	uint64_t delta_cycles;

	uint64_t last_real_timestamp;
	uint64_t delta_real_timestamp;

	bool negative_timestamp_warning_done;
};

BT_HIDDEN
bt_component_class_initialize_method_status pretty_init(
		bt_self_component_sink *component,
		bt_self_component_sink_configuration *config,
		const bt_value *params,
		void *init_method_data);

BT_HIDDEN
bt_component_class_sink_consume_method_status pretty_consume(
		bt_self_component_sink *component);

BT_HIDDEN
bt_component_class_sink_graph_is_configured_method_status pretty_graph_is_configured(
		bt_self_component_sink *component);

BT_HIDDEN
void pretty_finalize(bt_self_component_sink *component);

BT_HIDDEN
int pretty_print_event(struct pretty_component *pretty,
		const bt_message *event_msg);

BT_HIDDEN
int pretty_print_discarded_items(struct pretty_component *pretty,
		const bt_message *msg);

BT_HIDDEN
void pretty_print_init(void);

#endif /* BABELTRACE_PLUGIN_TEXT_PRETTY_PRETTY_H */
