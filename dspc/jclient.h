/*  jack audio client helpers
    (c) Philippe Strauss, spring 2010  */


#ifndef _JACKC_H
#define _JACKC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <jack/jack.h>
#include <stdbool.h>

#define MAX_JACKC_CHAN 16
#define MAX_JACKC_PORTS_PATTERN_LEN 32


/* a simple state machine for this jack client */
typedef volatile enum {
    jkc_uninitialized,
    jkc_initialized,
    jkc_opened,
    jkc_running,
    jkc_exiting
} jack_client_state;

typedef enum {
    jkc_port_ok,
    jkc_port_unavailable
} jackc_port_status_t;

/*  fairly generic set of datas pertaining to the functioning of
    a jack client  */
typedef struct {
    /*  client name and server name should be allocated in a larger scope
        server_name is NULL most of the time  */
    char *client_name;
    char *server_name;
    jack_nframes_t buff_depth;
    int n_in;
    char p_in_pattern[MAX_JACKC_PORTS_PATTERN_LEN];
    jack_port_t *p_in[MAX_JACKC_CHAN];
    int n_out;
    char p_out_pattern[MAX_JACKC_PORTS_PATTERN_LEN];
    jack_port_t *p_out[MAX_JACKC_CHAN];
    // system port number to connect to
    int p_in_sysconn[MAX_JACKC_CHAN];
    int p_out_sysconn[MAX_JACKC_CHAN];
    bool sysconn_initialized;
    jack_client_t *client;
    jack_client_state client_state;
    jack_options_t options;
    jack_status_t status;
    int sfreq;
    int capt_latency, play_latency;    
} jackc;

// function prototype
void jackc_init_state(jackc *, char *, char *, int, int, char *, char *);
jack_status_t jackc_init_client(jackc *);
jackc_port_status_t jackc_init_ports(jackc *);
void jackc_init_sysconn_in(jackc *, int, int);
void jackc_init_sysconn_out(jackc *, int, int);
jackc_port_status_t jackc_connects(jackc *);
int jackc_pp_ports_noclient(void);
void jackc_shutdown(void *);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif                           
