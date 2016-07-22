#include <stdio.h>
#include <string.h>
#include <ao/ao.h>
#include <math.h>

#define BUF_SIZE 4096

int main(int argc, char **argv)
{
	ao_device *device;
	ao_sample_format format;
	int default_driver;
	char *buffer;
	int buf_size;
	int sample;
	float freq = 440.0;
	int i;

	/* -- Initialize -- */

	ao_initialize();

	/* -- Setup for default driver -- */

	//default_driver = ao_default_driver_id();
	default_driver = ao_driver_id("alsa");
	ao_option *opts=NULL;
	ao_append_option(&opts, "device", "dmix:0");
	

        memset(&format, 0, sizeof(format));
	format.bits = 16;
	format.channels = 2;
	format.rate = 44100;
	format.byte_format = AO_FMT_LITTLE;

	/* -- Open driver -- */
	device = ao_open_live(default_driver, &format, opts /* no options */);
	if (device == NULL) {
		fprintf(stderr, "Error opening device.\n");
		return 1;
	}

	/* -- Play some stuff -- */
	buf_size = format.bits/8 * format.channels * format.rate;
	buffer = calloc(buf_size,
			sizeof(char));
	memset(buffer, 0, buf_size);
	unsigned int pos=0;
	while(1){
		ao_play(device, buffer, buf_size);
	}
	/* -- Close and shutdown -- */
	ao_close(device);

	ao_shutdown();

  return (0);
}
