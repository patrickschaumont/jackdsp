#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "fir.h"

#include <jack/jack.h>

jack_port_t *input_port;
jack_port_t *output_port;
jack_client_t *client;

float fir_buffer[FIR_TAP_COUNT];

int process (jack_nframes_t nframes, void *arg) {
  jack_default_audio_sample_t *in, *out;
  
  in = jack_port_get_buffer (input_port, nframes);
  out = jack_port_get_buffer (output_port, nframes);

  for (jack_nframes_t i = 0; i < nframes; i++) {
    for (int j = FIR_TAP_COUNT - 1; j > 0; j--) {
      fir_buffer[j] = fir_buffer[j - 1];
    }
    fir_buffer[0] = in[i];  // Insert new sample
    
    float filtered_sample = 0.0;
    for (int j = 0; j < FIR_TAP_COUNT; j++) {
      filtered_sample += fir_buffer[j] * fir_coeffs[j];
    }
    
    out[i] = filtered_sample;  // Store output sample
  }
  
  return 0;      
}

void jack_shutdown (void *arg) {
	exit (1);
}

int main (int argc, char *argv[]) {
  const char **ports;
  const char *client_name = "simple";
  const char *server_name = NULL;
  jack_options_t options = JackNullOption;
  jack_status_t status;
  
  client = jack_client_open (client_name, options, &status, server_name);
  if (client == NULL) {
    fprintf (stderr, "jack_client_open() failed, status = 0x%2.0x\n", status);
    if (status & JackServerFailed) {
      fprintf (stderr, "Unable to connect to JACK server\n");
    }
    exit (1);
  }
  if (status & JackServerStarted) {
    fprintf (stderr, "JACK server started\n");
  }
  if (status & JackNameNotUnique) {
    client_name = jack_get_client_name(client);
    fprintf (stderr, "unique name `%s' assigned\n", client_name);
  }
  
  jack_set_process_callback (client, process, 0);

  jack_on_shutdown (client, jack_shutdown, 0);

  printf ("engine sample rate: %" PRIu32 "\n", jack_get_sample_rate (client));

  input_port = jack_port_register (client, "input",
				   JACK_DEFAULT_AUDIO_TYPE,
				   JackPortIsInput, 0);
  output_port = jack_port_register (client, "output",
				    JACK_DEFAULT_AUDIO_TYPE,
				    JackPortIsOutput, 0);
  
  if ((input_port == NULL) || (output_port == NULL)) {
    fprintf(stderr, "no more JACK ports available\n");
    exit (1);
  }
  
  if (jack_activate (client)) {
    fprintf (stderr, "cannot activate client");
    exit (1);
  }

  const char *usbin = "USB Audio Device Mono:capture_MONO";
  if (jack_connect (client,
		    usbin,
		    jack_port_name (input_port))) {
    fprintf (stderr, "cannot connect input ports\n");
  }
  
  ports = jack_get_ports (client, NULL, NULL,
			  JackPortIsPhysical|JackPortIsInput);
  if (ports == NULL) {
    fprintf(stderr, "no physical playback ports\n");
    exit (1);
  }

  const char *usbout = "USB Audio Device Analog Stereo:playback_FR";
  if (jack_connect (client,
		    jack_port_name (output_port),
		    usbout)) {
    fprintf (stderr, "cannot connect output ports\n");
  }
  
  sleep (-1);
  
  jack_client_close (client);
  exit (0);
}
