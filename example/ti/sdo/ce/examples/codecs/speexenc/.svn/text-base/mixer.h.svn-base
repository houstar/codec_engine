#ifndef AUDIO_MIXER_H
#define AUDIO_MIXER_H

typedef struct {
	int level;
	int mix_adj;				// adjustment level for mixbuf
	int last_mix_adj;			// last adjustment level
	int tx_level;
	int tx_adj_level;
}stAudioMixLevel;

void   mixinit();
void  mixprocess(const short* ch1, const short* ch2, short* out);
int   get_level(stAudioMixLevel* plevel, const short* frame_buffer, int samples_per_frame);
void  mix_frame(stAudioMixLevel* plevel, const short* mix_in_buffer, int* mix_out_buffer);
int   adjust_out_level(stAudioMixLevel* plevel, const int* mix_in_buffer, short* mix_out_buffer);

#endif	// AUDIO_MIXER_H
