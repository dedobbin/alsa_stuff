#include <stdio.h>
#include "alsa.h"
#include "string.h"

int main (int argc, char* argv[])
{
	if (argc == 1){
		printf("Please give input path\n");
		return 1;
	}

	char audio_input_path[100];
	strcpy(audio_input_path, argv[1]);

	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;

	if (!pcm_init_playback(&handle, &params, 44100)){
		fprintf(stderr, "unable to init pcm\n");
		return 1;
	}

	if (!play_audio_from_file(audio_input_path, handle, params)){
		fprintf(stderr, "unable to play audio\n");
		return 1;
	}

	pcm_close(handle);
	//snd_pcm_hw_params_free(params); //TODO: properly free, this doesn't work..

	return 0;
}