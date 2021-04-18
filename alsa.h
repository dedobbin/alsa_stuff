#pragma once

#define ALSA_PCM_NEW_HW_PARAMS_API // use newer alsa api
#include <alsa/asoundlib.h>

int open_pcm(snd_pcm_t **handle, snd_pcm_hw_params_t **params);
int play_audio(char* input_path, snd_pcm_t *handle, snd_pcm_hw_params_t *params);
void close_pcm(snd_pcm_t *handle, snd_pcm_hw_params_t *params);
