#pragma once

#define ALSA_PCM_NEW_HW_PARAMS_API // use newer alsa api
#include <alsa/asoundlib.h>

int pcm_init_playback(snd_pcm_t **handle, snd_pcm_hw_params_t **params, int bitrate);
int play_audio_from_file(char* input_path, snd_pcm_t *handle, snd_pcm_hw_params_t *params);
void pcm_close(snd_pcm_t *handle);
