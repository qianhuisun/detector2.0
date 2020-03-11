# [Babeltrace2](https://babeltrace.org/)

## Version We Are Using

#### 1. source

* Tarball: [Babeltrace 2.0.0](https://www.efficios.com/files/babeltrace/babeltrace-2.0.0.tar.bz2)
* Github: [Babeltrace 2.0](https://github.com/efficios/babeltrace/tree/master)

#### 2. build and install

```cmd
$ BABELTRACE_DEV_MODE=1 BABELTRACE_MINIMAL_LOG_LEVEL=TRACE ./configure
$ make
$ make install
```

> Some libraries may be missed on your machine. Please check the error message to "configure" if any.

##  Custom Plugin Component

### 1. Component Types

Babaltrace is a trace converter that consists of three components:
  * Source component
  * Filter component
  * Sink component

### 2. Run Babeltrace2 with Custom Component

> Step 1: Build the shared object for your plugin 

```cmd
$ cc mycomponent1.c -fPIC -c $(pkg-config --cflags babeltrace2)
$ ld mycomponent1.o mycomponent2.o -o myplugin.so -shared $(pkg-config --libs babeltrace2)
```

> Step 2: Run babeltrace2 with custom component.

```cmd
$ babeltrace2 --plugin-path=/path/to/plugin /path/to/ctf/trace --component=sink.myplugin.mycomponent1
```

> Here is a simple example of using a self-implemented sink component [sink.epitome.output](https://babeltrace.org/docs/v2.0/libbabeltrace2/example-simple-sink-cmp-cls.html).

### 3. Write Custom Component

> Here is how to get parameters from a babeltrace stardard trace object (bt_message *msg):

* Timestamp
```c
bt_message_event *event_msg = (void *)msg;
double timestamp1 = event_msg->default_cs->value_cycles; // timestamp1(ns)
double timestamp2 = event_msg->default_cs->ns_from_origin; // timestamp2(ns)
```

* Event name
```c
bt_event *event = msg->event;
bt_event_class *ec = event->class;
char *event_name = ec->name.value; // event name
```

* Context (this includes cpu_id, etc.)
```c
bt_event *event = msg->event;
bt_packet *packet = bt_event_borrow_packet_const(event);
bt_field *field = bt_packet_borrow_context_field_const(packet);
// Recursively
bt_field_class *fc = field->class;
bt_field_class_named_field_class_container *fc = (void *)fc;
bt_field_class_structure_member *member = fc->named_fcs->pdata[index];
bt_named_field_class *named_fc = (const void *) member;
bt_field_structure *struct_field = (void *) field;
bt_field *member_field = struct_field->fields->pdata[index];
char *context_name = named_fc->name->str; // context name, e.g. cpu_id
int context_value = member_field->value; // context value, e.g. 3
```

* Common context (this includes pid, tid, etc.)
```c
bt_event *event = msg->event;
bt_field *field = event->common_context_field;
// Recursively
bt_field_class *fc = field->class;
bt_field_class_named_field_class_container *fc = (void *)fc;
bt_field_class_structure_member *member = fc->named_fcs->pdata[index];
bt_named_field_class *named_fc = (const void *) member;
bt_field_structure *struct_field = (void *) field;
bt_field *member_field = struct_field->fields->pdata[index];
char *context_name = named_fc->name->str; // context name, e.g. pid
int context_value = member_field->value; // context value, e.g. 4939
```

* Special context
```c
bt_event *event = msg->event;
bt_field *field = event->specific_context_field;
// Recursively
// ...
```
> Special context has the same call graph as common context.

* Payload (this includes most other parameters)
```c
bt_event *event = msg->event;
bt_field *field = event->payload_field;
// Recursively
bt_field_class *fc = field->class;
bt_field_class_named_field_class_container *fc = (void *)fc;
bt_field_class_structure_member *member = fc->named_fcs->pdata[index];
bt_named_field_class *named_fc = (const void *) member;
bt_field_structure *struct_field = (void *) field;
bt_field *member_field = struct_field->fields->pdata[index];
char *payload_name = named_fc->name->str; // payload name, e.g. filename
char *payload_value = member_field->value; // payload value, e.g. /etc/crontab
```

## Babeltrace2 Call Graph

### 1. Configure Arguments

```
In int main(int argc, const char **argv):
In struct bt_config *bt_config_cli_args_create_with_default(int argc, const char *argv[], int *retcode, const bt_interrupter *interrupter):
In struct bt_config *bt_config_cli_args_create(int argc, const char *argv[], int *retcode, bool omit_system_plugin_path, bool omit_home_plugin_path, const bt_value *initial_plugin_paths, const bt_interrupter *interrupter):
In struct bt_config *bt_config_convert_from_args(int argc, const char *argv[], int *retcode, const bt_value *plugin_paths, int *default_log_level, const bt_interrupter *interrupter):
```

### 2. Run Babeltrace2

```
In int main(int argc, const char **argv):
In static enum bt_cmd_status cmd_run(struct bt_config *cfg):
In enum bt_graph_run_status bt_graph_run(struct bt_graph *graph):
In static inline int consume_no_check(struct bt_graph *graph):
In static inline int consume_sink_node(struct bt_graph *graph, GList *node):
In static inline int consume_graph_sink(struct bt_component_sink *comp):
```