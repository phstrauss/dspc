/*  jack audio client helpers
    (c) Philippe Strauss, spring 2010  */


#include <stdio.h>
#include <stdlib.h>
#include <jack/jack.h>
#include <assert.h>
#include <string.h> // strncpy

#include <dspc/dumb_alloc2d.h>
#include <dspc/perr.h>
#include <dspc/jclient.h>


// WW : think errhandling as a lib

void jackc_init_state(jackc *jkc, char *clientname, char *servername,
                      int ports_in, int ports_out,
                      char *pattern_in, char *pattern_out) {
    jkc->client_name = clientname;
    jkc->server_name = servername;
    jkc->options = JackNullOption; 
    jkc->client_state = jkc_initialized;
    jkc->n_in = ports_in;
    if (pattern_in != NULL)
        strncpy(jkc->p_in_pattern, pattern_in, MAX_JACKC_PORTS_PATTERN_LEN);
    else
        strncpy(jkc->p_in_pattern, "in_%d", MAX_JACKC_PORTS_PATTERN_LEN);
    jkc->n_out = ports_out;
    if (pattern_out != NULL)
        strncpy(jkc->p_out_pattern, pattern_out, MAX_JACKC_PORTS_PATTERN_LEN);
    else
        strncpy(jkc->p_out_pattern, "out_%d", MAX_JACKC_PORTS_PATTERN_LEN);
    int i;
    for (i = 0; i < MAX_JACKC_CHAN; ++i) {
        // minus one means stay unconnected
        jkc->p_in_sysconn[i] = -1; jkc->p_out_sysconn[i] = -1;
    }
    jkc->sysconn_initialized = false;
}

jack_status_t jackc_init_client(jackc *jkc) {
    jkc->client = jack_client_open(jkc->client_name, jkc->options, &(jkc->status), jkc->server_name);
    if (jkc->client == NULL) {
        eprintf("jackc_init_client(): jack_client_open() failed, status = 0x%x\n", jkc->status);
         if (jkc->status & JackServerFailed)
             eprintf("jackc_init_client(): unable to connect to jack server\n");
         return jkc->status;
    }
    if (jkc->status & JackServerStarted)
        dprintf("jackc_init_client(): jack server started\n");
    if (jkc->status & JackNameNotUnique) {
        jkc->client_name = jack_get_client_name(jkc->client);
        printf("jackc_init_client(): unique name `%s' assigned\n", jkc->client_name);
    }
    jkc->client_state = jkc_opened;
    jkc->sfreq = jack_get_sample_rate(jkc->client);
    jkc->buff_depth = jack_get_buffer_size(jkc->client);
    /*  jclient.c:58:5: warning: 'jack_port_get_latency' is deprecated (declared at /usr/local/include/jack/jack.h:1172) [-Wdeprecated-declarations]
        jclient.c:59:5: warning: 'jack_port_get_latency' is deprecated (declared at /usr/local/include/jack/jack.h:1172) [-Wdeprecated-declarations]
        It should not be used in new code, and should be replaced by jack_port_get_latency_range() in any existing use cases.
        see code example in jack_port_get_latency_range header in jack.h
        BUT : This function should ONLY be used inside a latency callback!  */
    jkc->capt_latency = jack_port_get_latency(jack_port_by_name(jkc->client, "system:capture_1"));
    jkc->play_latency = jack_port_get_latency(jack_port_by_name(jkc->client, "system:playback_1"));
    return jkc->status;
}

jackc_port_status_t jackc_init_ports(jackc *jkc) {
    int i;
    char p_name[8]; // p_name: out_1; out_2 etc...
    /* create ports */
    for (i = 0; i < jkc->n_in; ++i) {
        sprintf(p_name, jkc->p_in_pattern, (i + 1));
        jkc->p_in[i] = jack_port_register(jkc->client, p_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
        if (jkc->p_in[i] == NULL) {
            dprintf("jackc_init_ports(): no more jack input ports available\n");
            return jkc_port_unavailable;
        }
    }
    for (i = 0; i < jkc->n_out; ++i) {
        sprintf(p_name, jkc->p_out_pattern, (i + 1));
        jkc->p_out[i] = jack_port_register(jkc->client, p_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
        if (jkc->p_out[i] == NULL) {
            dprintf("jackc_init_ports(): no more jack output ports available\n");
            return jkc_port_unavailable;
        }
    }
    return jkc_port_ok;
}

int jackc_count_ports(const char **ports) {
    int i = 0;

    while (ports[i] != NULL)
        i++;
    return i;
}

void pretty_print_ports(const char **ports_in, const char **ports_out) {
    int i = 0, in_remain = 1, out_remain = 1;
    char pin_str[80], pout_str[80];

    while ((ports_in[i] != NULL) || (ports_out[i] != NULL)) {
        if (ports_in[i] == NULL)
            in_remain = 0;
        if (ports_out[i] == NULL)
            out_remain = 0;

        if (in_remain) {
            sprintf(pin_str, "i%-2d : %-32s", i + 1, ports_in[i]);
        } else {
            pin_str[0] = '\0';
        }

        if (out_remain) {
            sprintf(pout_str, "o%-2d : %-32s", i + 1, ports_out[i]);
        } else {
            pout_str[0] = '\0';
        }

        printf("%s    %s\n", pin_str, pout_str);
        i++;
    }
}

/* FIX : return type : check jack errcode and our enum jackc_port_status_t
   TODO : split query and pprinting, query a lib proc */
int jackc_pp_ports_noclient(void) {
    jack_client_t *client;
    jack_options_t options = JackNullOption;
    jack_status_t status;
    const char *client_name = "jackc_listports";
    const char **ports_in, **ports_out;

    client = jack_client_open(client_name, options, &status, NULL);
    if (client == NULL) {
        eprintf("jack_client_open() failed, status = 0x%2.0x\n", status);
        if (status & JackServerFailed)
            eprintf("Unable to connect to JACK server\n");
        return status;
    }
    if (status & JackServerStarted)
        eprintf("JACK server started\n");
    if (status & JackNameNotUnique) {
        client_name = jack_get_client_name(client);
        eprintf("unique name `%s' assigned\n", client_name);
    }

    ports_in =
        jack_get_ports(client, NULL, NULL,
                       JackPortIsPhysical | JackPortIsOutput);
    if (ports_in == NULL) {
        eprintf("no physical capture ports\n");
        return jkc_port_unavailable;
    }
    ports_out = jack_get_ports(client, NULL, NULL, JackPortIsPhysical | JackPortIsInput);
    if (ports_out == NULL) {
        eprintf("no physical playback ports\n");
        return jkc_port_unavailable;
    }

    printf("\nports available on jackd:\n");
    pretty_print_ports(ports_in, ports_out);
    printf("\n");

    jack_on_shutdown(client, jackc_shutdown, 0);

    jack_client_close(client);
}

/*  system (AD/DA I/O card) connection init procedures  */
void jackc_init_sysconn_in(jackc* jkc, int jkcp_index, int sysport_nr) {
    assert(jkcp_index < MAX_JACKC_CHAN);
    // jack port numbers starts at 1
    assert(sysport_nr-1 >= 0); // FAILURE
    jkc->p_in_sysconn[jkcp_index] = sysport_nr-1;
    jkc->sysconn_initialized = true;
}

void jackc_init_sysconn_out(jackc* jkc, int jkcp_index, int sysport_nr) {
    assert(jkcp_index < MAX_JACKC_CHAN);
    assert(sysport_nr-1 >= 0);
    jkc->p_out_sysconn[jkcp_index] = sysport_nr-1;
    jkc->sysconn_initialized = true;
}

jackc_port_status_t jackc_connects(jackc *jkc) {
    const char **ports_in, **ports_out;
    int i;
    /* Connect the ports.  You can't do this before the client is
     * activated, because we can't make connections to clients
     * that aren't running.  Note the confusing (but necessary)
     * orientation of the driver backend ports: playback ports are
     * "input" to the backend, and capture ports are "output" from it.
     */
    ports_in = jack_get_ports(jkc->client, NULL, NULL, JackPortIsPhysical | JackPortIsOutput);
    if (ports_in == NULL) {
        dprintf("jackc_connects(): no physical capture ports\n");
        // FIX: a D/A board with no A/D is conceivable, albeit rare
        return jkc_port_unavailable;
    }
    ports_out = jack_get_ports(jkc->client, NULL, NULL, JackPortIsPhysical | JackPortIsInput);
    if (ports_out == NULL) {
        dprintf("jackc_connects(): no physical playback ports\n");
        free(ports_in);
        return jkc_port_unavailable;
    }

    dprintf("\nports available on jackd:\n");
#ifdef DEBUG
    pretty_print_ports(ports_in, ports_out);
#endif
    dprintf("\n");

    // for sanity check
    int n_sysp_in = jackc_count_ports(ports_in);
    int n_sysp_out = jackc_count_ports(ports_out);
    dprintf("jackc_connects: system port counts: in=%d, out=%d\n", n_sysp_in, n_sysp_out);
    int port_index;
    for (i = 0; i < jkc->n_in; ++i) {
        // FIX: cleaner than assert (freeing resources)
        assert(jkc->p_in_sysconn[i] < n_sysp_in);
        if (jkc->sysconn_initialized) {
            port_index = jkc->p_in_sysconn[i];
            if (port_index >= 0) // -1 means don't connect at all
                if (jack_connect(jkc->client, ports_in[port_index], jack_port_name(jkc->p_in[i])))
                    eprintf("cannot connect all input ports\n");
        } else {
            // dumb autoconnect from port 1,2,...
            if (jack_connect(jkc->client, ports_in[i], jack_port_name(jkc->p_in[i])))
                eprintf("cannot connect all input ports\n");
        }
    }
    for (i = 0; i < jkc->n_out; ++i) {
        assert(jkc->p_out_sysconn[i] < n_sysp_out);
        if (jkc->sysconn_initialized) {
            port_index = jkc->p_out_sysconn[i];
            if (port_index >= 0) // -1 means don't connect at all
                if (jack_connect(jkc->client, jack_port_name(jkc->p_out[i]), ports_out[port_index]))
                    eprintf("cannot connect all output ports\n");
        } else {
            // dumb autoconnect from port 1,2,...
            if (jack_connect(jkc->client, jack_port_name(jkc->p_out[i]), ports_out[i]))
                eprintf("cannot connect all input ports\n");
        }
    }

    free(ports_in);
    free(ports_out);
    return jkc_port_ok;
}

// FIX: think part of a lib
void jackc_shutdown(void *arg) {
    exit(1);
}
