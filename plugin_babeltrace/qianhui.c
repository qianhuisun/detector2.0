#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <babeltrace2/babeltrace.h>
// Qianhui: 02/12/2020 Begin
#include <stdbool.h>
// Qianhui: 02/12/2020 End

 
/* Sink component's private data */
struct qianhui_out {
    /* Upstream message iterator (owned by this) */
    bt_message_iterator *message_iterator;
 
    /* Current event message index */
    uint64_t index;
};

// Qianhui: 02/12/2020 Begin
/* Define customed field structure for Payload, Context, etc. */
// Qianhui: 02/12/2020 End


// Qianhui: 02/12/2020 Begin
/* Copied from write.c in sink.text.details component */
static inline
void format_uint(char *buf, uint64_t value, unsigned int base)
{
    const char *spec = "%" PRIu64;
    char *buf_start = buf;
    unsigned int digits_per_group = 3;
    char sep = ',';

    switch (base) {
    case 2:
    case 16:
        /* TODO: Support binary format */
        spec = "%" PRIx64;
        strcpy(buf, "0x");
        buf_start = buf + 2;
        digits_per_group = 4;
        sep = ':';
        break;
    case 8:
        spec = "%" PRIo64;
        strcpy(buf, "0");
        buf_start = buf + 1;
        sep = ':';
        break;
    case 10:
        break;
    default:
        break;
    }

    sprintf(buf_start, spec, value);
}

static inline
void format_int(char *buf, int64_t value, unsigned int base)
{
    const char *spec = "%" PRIu64;
    char *buf_start = buf;
    unsigned int digits_per_group = 3;
    char sep = ',';
    uint64_t abs_value = value < 0 ? (uint64_t) -value : (uint64_t) value;

    if (value < 0) {
        buf[0] = '-';
        buf_start++;
    }

    switch (base) {
    case 2:
    case 16:
        /* TODO: Support binary format */
        spec = "%" PRIx64;
        strcpy(buf_start, "0x");
        buf_start += 2;
        digits_per_group = 4;
        sep = ':';
        break;
    case 8:
        spec = "%" PRIo64;
        strcpy(buf_start, "0");
        buf_start++;
        sep = ':';
        break;
    case 10:
        break;
    default:
        break;
    }

    sprintf(buf_start, spec, abs_value);
}
// Qianhui: 02/12/2020 End

 
/*
 * Initializes the sink component.
 */
static
bt_component_class_initialize_method_status qianhui_out_initialize(
        bt_self_component_sink *self_component_sink,
        bt_self_component_sink_configuration *configuration,
        const bt_value *params, void *initialize_method_data)
{
    /* Allocate a private data structure */
    struct qianhui_out *qianhui_out = malloc(sizeof(*qianhui_out));
 
    /* Initialize the first event message's index */
    qianhui_out->index = 1;
 
    /* Set the component's user data to our private data structure */
    bt_self_component_set_data(
        bt_self_component_sink_as_self_component(self_component_sink),
        qianhui_out);
 
    /*
     * Add an input port named `in` to the sink component.
     *
     * This is needed so that this sink component can be connected to a
     * filter or a source component. With a connected upstream
     * component, this sink component can create a message iterator
     * to consume messages.
     */
    bt_self_component_sink_add_input_port(self_component_sink,
        "in", NULL, NULL);
 
    return BT_COMPONENT_CLASS_INITIALIZE_METHOD_STATUS_OK;
}
 
/*
 * Finalizes the sink component.
 */
static
void qianhui_out_finalize(bt_self_component_sink *self_component_sink)
{
    /* Retrieve our private data from the component's user data */
    struct qianhui_out *qianhui_out = bt_self_component_get_data(
        bt_self_component_sink_as_self_component(self_component_sink));
 
    /* Free the allocated structure */
    free(qianhui_out);
}
 
/*
 * Called when the trace processing graph containing the sink component
 * is configured.
 *
 * This is where we can create our upstream message iterator.
 */
static
bt_component_class_sink_graph_is_configured_method_status
qianhui_out_graph_is_configured(bt_self_component_sink *self_component_sink)
{
    /* Retrieve our private data from the component's user data */
    struct qianhui_out *qianhui_out = bt_self_component_get_data(
        bt_self_component_sink_as_self_component(self_component_sink));
 
    /* Borrow our unique port */
    bt_self_component_port_input *in_port =
        bt_self_component_sink_borrow_input_port_by_index(
            self_component_sink, 0);
 
    /* Create the uptream message iterator */
    bt_message_iterator_create_from_sink_component(self_component_sink,
        in_port, &qianhui_out->message_iterator);
 
    return BT_COMPONENT_CLASS_SINK_GRAPH_IS_CONFIGURED_METHOD_STATUS_OK;
}

// Qianhui: 02/12/2020 Begin
/* Print payload field. */
static
void print_field(const bt_field *field, const char *name) {
    bt_field_class_type field_class_type = bt_field_get_class_type(field);
    const bt_field_class *field_class;
    char buf[64];
    /* Write field's name if it's not NULL */
    if (name) {
        printf("\"%s\":", name);
    }
    /* Write field's value */
    if (field_class_type == BT_FIELD_CLASS_TYPE_BOOL) {
        bt_bool prop_value = bt_field_bool_get_value(field);
        if (prop_value) {
            printf("true");
        } else {
            printf("false");
        }
    } else if (field_class_type == BT_FIELD_CLASS_TYPE_BIT_ARRAY) {
        format_uint(buf, bt_field_bit_array_get_value_as_integer(field), 16);
        printf("%s", buf);
    } else if (bt_field_class_type_is(field_class_type, BT_FIELD_CLASS_TYPE_INTEGER)) {
        field_class = bt_field_borrow_class_const(field);
        bt_field_class_integer_preferred_display_base base = bt_field_class_integer_get_preferred_display_base(field_class);
        unsigned int fmt_base;
        switch (base) {
        case BT_FIELD_CLASS_INTEGER_PREFERRED_DISPLAY_BASE_DECIMAL:
            fmt_base = 10;
            break;
        case BT_FIELD_CLASS_INTEGER_PREFERRED_DISPLAY_BASE_OCTAL:
            fmt_base = 8;
            break;
        case BT_FIELD_CLASS_INTEGER_PREFERRED_DISPLAY_BASE_BINARY:
            fmt_base = 2;
            break;
        case BT_FIELD_CLASS_INTEGER_PREFERRED_DISPLAY_BASE_HEXADECIMAL:
            fmt_base = 16;
            break;
        default:
            return;
        }
        if (bt_field_class_type_is(field_class_type, BT_FIELD_CLASS_TYPE_UNSIGNED_INTEGER)) {
            format_uint(buf, bt_field_integer_unsigned_get_value(field), fmt_base);
            printf("%s", buf);
        } else {
            format_uint(buf, bt_field_integer_signed_get_value(field), fmt_base);
            printf("%s", buf);
        }
    } else if (field_class_type == BT_FIELD_CLASS_TYPE_SINGLE_PRECISION_REAL) {
        printf("%f", (double) bt_field_real_single_precision_get_value(field));
    } else if (field_class_type == BT_FIELD_CLASS_TYPE_DOUBLE_PRECISION_REAL) {
        printf("%f", (double) bt_field_real_double_precision_get_value(field));
    } else if (field_class_type == BT_FIELD_CLASS_TYPE_STRING) {
        printf("\"%s\"", bt_field_string_get_value(field));
    } else if (field_class_type == BT_FIELD_CLASS_TYPE_STRUCTURE) {
        field_class = bt_field_borrow_class_const(field);
        uint64_t member_count = bt_field_class_structure_get_member_count(field_class);
        /* Print JSON object left brace */
        printf("{ ");
        for (int i = 0; i < member_count; ++i) {
            const bt_field_class_structure_member *field_class_structure_member =
                bt_field_class_structure_borrow_member_by_index_const(field_class, i);
            const bt_field *member_field =
                bt_field_structure_borrow_member_field_by_index_const(field, i);
            print_field(member_field, bt_field_class_structure_member_get_name(field_class_structure_member));
            if (i < member_count - 1) {
                printf(", ");
            }
        }
        /* Print JSON object right brace */
        printf(" }");
    } else if (bt_field_class_type_is(field_class_type, BT_FIELD_CLASS_TYPE_ARRAY)) {
        uint64_t element_count = bt_field_array_get_length(field);
        /* Print JSON array left bracket */
        printf("[ ");
        for (int i = 0; i < element_count; ++i) {
            const bt_field *element_field = 
                bt_field_array_borrow_element_field_by_index_const(field, i);
            print_field(element_field, NULL);
            if (i < element_count - 1) {
                printf(", ");
            }
        }
        /* Print JSON array right bracket */
        printf(" ]");
    } else if (bt_field_class_type_is(field_class_type, BT_FIELD_CLASS_TYPE_OPTION)) {
        const bt_field *option_field =
            bt_field_option_borrow_field_const(field);
        if (option_field) {
            print_field(option_field, NULL);
        }
    } else if (bt_field_class_type_is(field_class_type, BT_FIELD_CLASS_TYPE_VARIANT)) {
        const bt_field *variant_field =
            bt_field_variant_borrow_selected_option_field_const(field);
        print_field(variant_field, NULL);
    } 
}
// Qianhui: 02/12/2020 End

 
/*
 * Prints a line for `message`, if it's an event message, to the
 * standard output.
 */
static
void print_message(struct qianhui_out *qianhui_out, const bt_message *message)
{
    /* Discard if it's not an event message */
    if (bt_message_get_type(message) != BT_MESSAGE_TYPE_EVENT) {
        goto end;
    }
 
    /* Borrow the event message's event and its class */
    const bt_event *event = bt_message_event_borrow_event_const(message);
    const bt_event_class *event_class = bt_event_borrow_class_const(event);
 
    /* Get the payload field members */
    const bt_field *payload_field = bt_event_borrow_payload_field_const(event);
    uint64_t member_count = bt_field_class_structure_get_member_count(bt_field_borrow_class_const(payload_field));
 
    /* Write a corresponding line to the standard output */
    printf("#%" PRIu64 ": %s (%" PRIu64 " payload member%s)\n",
        qianhui_out->index, bt_event_class_get_name(event_class),
        member_count, member_count == 1 ? "" : "s");
        
    // Qianhui: 02/12/2020 Begin
    /* Extract payload field to self-defined field structure. */
    // TODO
    // Qianhui: 02/12/2020 End
    // Qianhui: 02/12/2020 Begin
    /* Print payload field. */
    print_field(payload_field, NULL);
    printf("\n");
    // Qianhui: 02/12/2020 End

 
    /* Increment the current event message's index */
    qianhui_out->index++;
 
end:
    return;
}
 
/*
 * Consumes a batch of messages and writes the corresponding lines to
 * the standard output.
 */
bt_component_class_sink_consume_method_status qianhui_out_consume(
        bt_self_component_sink *self_component_sink)
{
    bt_component_class_sink_consume_method_status status =
        BT_COMPONENT_CLASS_SINK_CONSUME_METHOD_STATUS_OK;
 
    /* Retrieve our private data from the component's user data */
    struct qianhui_out *qianhui_out = bt_self_component_get_data(
        bt_self_component_sink_as_self_component(self_component_sink));
 
    /* Consume a batch of messages from the upstream message iterator */
    bt_message_array_const messages;
    uint64_t message_count;
    bt_message_iterator_next_status next_status =
        bt_message_iterator_next(qianhui_out->message_iterator, &messages,
            &message_count);
 
    switch (next_status) {
    case BT_MESSAGE_ITERATOR_NEXT_STATUS_END:
        /* End of iteration: put the message iterator's reference */
        bt_message_iterator_put_ref(qianhui_out->message_iterator);
        status = BT_COMPONENT_CLASS_SINK_CONSUME_METHOD_STATUS_END;
        goto end;
    case BT_MESSAGE_ITERATOR_NEXT_STATUS_AGAIN:
        status = BT_COMPONENT_CLASS_SINK_CONSUME_METHOD_STATUS_AGAIN;
        goto end;
    case BT_MESSAGE_ITERATOR_NEXT_STATUS_MEMORY_ERROR:
        status = BT_COMPONENT_CLASS_SINK_CONSUME_METHOD_STATUS_MEMORY_ERROR;
        goto end;
    case BT_MESSAGE_ITERATOR_NEXT_STATUS_ERROR:
        status = BT_COMPONENT_CLASS_SINK_CONSUME_METHOD_STATUS_ERROR;
        goto end;
    default:
        break;
    }
 
    /* For each consumed message */
    for (uint64_t i = 0; i < message_count; i++) {
        /* Current message */
        const bt_message *message = messages[i];
 
        /* Print line for current message if it's an event message */
        print_message(qianhui_out, message);
 
        /* Put this message's reference */
        bt_message_put_ref(message);
    }
 
end:
    return status;
}
 
/* Mandatory */
BT_PLUGIN_MODULE();
 
/* Define the `qianhui` plugin */
BT_PLUGIN(qianhui);
 
/* Define the `output` sink component class */
BT_PLUGIN_SINK_COMPONENT_CLASS(output, qianhui_out_consume);
 
/* Set some of the `output` sink component class's optional methods */
BT_PLUGIN_SINK_COMPONENT_CLASS_INITIALIZE_METHOD(output,
    qianhui_out_initialize);
BT_PLUGIN_SINK_COMPONENT_CLASS_FINALIZE_METHOD(output, qianhui_out_finalize);
BT_PLUGIN_SINK_COMPONENT_CLASS_GRAPH_IS_CONFIGURED_METHOD(output,
    qianhui_out_graph_is_configured);