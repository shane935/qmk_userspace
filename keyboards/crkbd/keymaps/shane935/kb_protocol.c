#include "kb_protocol.h"

#include <string.h>

void kb_state_report(uint8_t *buf, const kb_state_t *in) {
    memset(buf, 0, KB_REPORT_SIZE);
    buf[0] = KB_MAGIC;
    buf[1] = KB_VERSION;
    buf[2] = in->seq;
    buf[3] = in->ack;
    buf[4] = in->type;
    buf[5] = in->intent;
    buf[6] = in->intent_arg;
    // Always sent, not just with an intent: the host matches the status it
    // reports back against the last nonce it saw.
    buf[7]  = in->nonce;
    buf[8]  = in->mode;
    buf[9]  = in->mod;
    buf[10] = in->os | (in->os_override ? KB_OS_OVERRIDE : 0);
    buf[11] = in->host_alive ? KB_FLAG_HOST_ALIVE : 0;
    buf[12] = in->event;
    buf[13] = in->event_arg;
}

bool kb_context_parse(const uint8_t *data, uint8_t length, kb_context_t *out) {
    if (length < KB_REPORT_SIZE || data[0] != KB_MAGIC || data[1] != KB_VERSION || data[4] != KB_CONTEXT) {
        return false;
    }
    out->seq           = data[2];
    out->ack           = data[3];
    out->os            = data[5];
    out->program       = data[6];
    out->tmux_bits     = data[7];
    out->transcript    = data[8];
    out->phase         = data[9];
    out->perm          = data[10];
    out->intent_nonce  = data[11];
    out->intent_status = data[12];
    out->window        = data[13];
    out->pane          = data[14];
    // The wire field is 16 bytes and may use all of them, so the terminator is
    // the seventeenth byte and belongs to us rather than to the host.
    memcpy(out->label, &data[15], 16);
    out->label[16] = '\0';
    return true;
}

uint8_t kb_tmux_event_arg(uint8_t row, bool shift, bool ctrl, bool alt) {
    return row | (shift ? 0x10 : 0) | (ctrl ? 0x20 : 0) | (alt ? 0x40 : 0);
}
