#include "alsa.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

// regarding bitrate, bits per second, 44100 is CD quality
int pcm_init_playback(snd_pcm_t **handle, snd_pcm_hw_params_t **params, int bitrate)
{
	int rc = snd_pcm_open(handle, "default",
						SND_PCM_STREAM_PLAYBACK, 0);
	if (rc < 0) {
		fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
		return 0;
	}
	snd_pcm_hw_params_alloca(params);
	snd_pcm_hw_params_any(*handle, *params); // Fill params with default values
	snd_pcm_hw_params_set_access(*handle, *params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(*handle, *params, SND_PCM_FORMAT_S16_LE); //16bit signed, little endian
	snd_pcm_hw_params_set_channels(*handle, *params, 2);

	int dir = 0;
	snd_pcm_hw_params_set_rate_near(*handle, *params, &bitrate, &dir);

	// Frame is 1 sample for all channels, period is data that will be send to play back buffer together
	//	Small period means CPU 'wakes up' more often, so more power consuming, but lower drop out safety because you don't fill buffer as often.
	snd_pcm_uframes_t frames = 32;
	snd_pcm_hw_params_set_period_size_near(*handle, *params, &frames, &dir);

	rc = snd_pcm_hw_params(*handle, *params);
	if (rc < 0) {
		fprintf(stderr, "unable to set hw parameters to driver: %s\n", snd_strerror(rc));
		return 0;
	}

	return 1;
}

int play_audio_from_file(char* input_path, snd_pcm_t *handle, snd_pcm_hw_params_t *params)
{
	int dir = 0;
	snd_pcm_uframes_t frames;
	snd_pcm_hw_params_get_period_size(params, &frames, &dir);

	int n_channels;
	snd_pcm_hw_params_get_channels(params, &n_channels);

 	snd_pcm_format_t format;
	int sample_size = -1;
	snd_pcm_hw_params_get_format(params, &format);
	switch(format){
		case SND_PCM_FORMAT_S16_LE:
			sample_size = 2;
			break;
		default:
			fprintf(stderr, "play_audio: unsuported format\n");
			return 0;
	}
	
	// Get data size of 1 period
	int size = frames * (n_channels + sample_size);

	char* audio_buffer = (char *) malloc(size);

	unsigned int val;
	snd_pcm_hw_params_get_period_time(params, &val, &dir);

	FILE* fp = fopen(input_path, "r");
	for (;;) {
		int rc = fread(audio_buffer, 1, size, fp);
		if (rc == 0) {
			fprintf(stderr, "end of file on input\n");
			break;
		} else if (rc != size) {
			fprintf(stderr, "short read: read %d bytes\n", rc);
		}

		rc = snd_pcm_writei(handle, audio_buffer, frames);
		if (rc == -EPIPE) {
			fprintf(stderr, "underrun occurred\n");
			snd_pcm_prepare(handle);
		} else if (rc < 0) {
			fprintf(stderr,"error from writei: %s\n", snd_strerror(rc));
		} else if (rc != (int)frames) {
			fprintf(stderr,"short write, write %d frames\n", rc);
		}
		//usleep(500);//create underrun
	}
	fclose(fp);
	free(audio_buffer);

	return 1;
}

void pcm_close(snd_pcm_t *handle)
{
	snd_pcm_drain(handle);
	snd_pcm_close(handle);
}