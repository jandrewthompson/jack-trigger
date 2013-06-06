/*
 * This isn't the copyright text you're looking for
 */

#include <jack/jack.h>
#include <jack/midiport.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

jack_client_t *client;
jack_port_t *output_port;
jack_port_t *input_port;

int note_frq;
int gate;
int wait_cnt = 0;
jack_default_audio_sample_t threshold = 0.5f;

void usage()
{
	fprintf(stderr, "usage: jack_trigger name note gate_samples threshold\n"); 
	fprintf(stderr, "eg: jack_trigger KickTrigger 29 30 0.5 \n");
	fprintf(stderr, "will play an f3 note on input's greater than 0.5 with a 30 sample-length gate in between\n");
}

int process(jack_nframes_t nframes, void *arg)
{
    jack_default_audio_sample_t *in = (jack_default_audio_sample_t *) jack_port_get_buffer(input_port, nframes);
    void* port_buf = jack_port_get_buffer(output_port, nframes);
    unsigned char* buffer;
    jack_midi_clear_buffer(port_buf);

    int i;
    for(i=0; i < nframes ;i++) 
    {
        if(in[i] > threshold && wait_cnt == 0) 
        {
            wait_cnt = gate;

            printf("\n%1f\n",in[i]);
            buffer = jack_midi_event_reserve(port_buf, 1, 3);
            buffer[2] = 50;
            buffer[1] = note_frq;
            buffer[0] = 0x90;
        }
        if(wait_cnt > 0) 
        {
            wait_cnt = --wait_cnt;
            //printf("Waiting: %d\n",wait_cnt);
        }
    }

    return 0;
}

int main(int narg, char **args)
{
    int i;
    jack_nframes_t nframes;
    if(narg<5)
    {
        usage();
        exit(1);
    }
    if((client = jack_client_open (args[1], JackNullOption, NULL)) == 0)
    {
        fprintf (stderr, "jack server not running?\n");
        return 1;
    }
    jack_set_process_callback (client, process, 0);
    input_port  = jack_port_register (client, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    output_port = jack_port_register (client, "out", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);

    note_frq = atoi(args[2]);
    gate = atoi(args[3]);
    threshold = atof(args[4]);

    if (jack_activate(client))
    {
        fprintf (stderr, "cannot activate client");
        return 1;
    }

    while (1)
    {
        sleep(1);
    };

}
