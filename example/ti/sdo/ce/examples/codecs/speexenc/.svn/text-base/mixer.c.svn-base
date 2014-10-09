#include "mixer.h"
#include "mix_linear2ulaw_table.h"

const int kSamplesPerFrame = 320;
const int kSampleRate      = 16000;

const int kFifoBufferCount = 10;

short sMixSamplesData[320] = {0};	  // mix channels except key channel
int   iMixSamplesData[320] = {0};
	
/* These are settings to control the adaptivity of changes in the
 * signal level of the ports, so that sudden change in signal level
 * in the port does not cause misaligned signal (which causes noise).
 */
#define AUDIO_NORMAL_LEVEL			128
	
#define AUDIO_ATTACK_A    (kSampleRate / kSamplesPerFrame)
#define AUDIO_ATTACK_B    1
#define AUDIO_DECAY_A	  0
#define AUDIO_DECAY_B	  1
	
#define AUDIO_SIMPLE_AGC(last, target)									\
    if (target >= last)													\
		target = (AUDIO_ATTACK_A * (last + 1) + AUDIO_ATTACK_B * target) / (AUDIO_ATTACK_A + AUDIO_ATTACK_B); \
    else																\
		target = (AUDIO_DECAY_A * last + AUDIO_DECAY_B * target) / (AUDIO_DECAY_A + AUDIO_DECAY_B)
	
#define AUDIO_MAX_LEVEL   (32767)
#define AUDIO_MIN_LEVEL   (-32768)
	
#define AUDIO_IS_OVERFLOW(s) ((s > AUDIO_MAX_LEVEL) || (s < AUDIO_MIN_LEVEL))
#define audio_mix_linear2ulaw(pcm_value)  audio_mix_linear2ulaw_table[(((short)pcm_value) >> 2) & 0x3fff]

stAudioMixLevel ch1, ch2, mixch;

void mixinit()
{
    ch1.level        = 0;
	ch1.mix_adj      = AUDIO_NORMAL_LEVEL;
	ch1.last_mix_adj = AUDIO_NORMAL_LEVEL;
	ch1.tx_level     = 0;
	ch1.tx_adj_level = AUDIO_NORMAL_LEVEL;

	ch2.level        = 0;
	ch2.mix_adj      = AUDIO_NORMAL_LEVEL;
	ch2.last_mix_adj = AUDIO_NORMAL_LEVEL;
	ch2.tx_level     = 0;
	ch2.tx_adj_level = AUDIO_NORMAL_LEVEL;
}

void mixprocess(const short* channel1, const short* channel2, short* mix)
{
	int length = 0;

	memset(iMixSamplesData, 0, kSamplesPerFrame * sizeof(int));
	//memset(sMixSamplesData, 0, kSamplesPerFrame * sizeof(short));

    get_level(&ch1, channel1, kSamplesPerFrame);
	get_level(&ch2, channel2, kSamplesPerFrame);

	if (ch1.level > 0) {
		mix_frame(&ch1, channel1, iMixSamplesData);
	}
	if (ch2.level > 0) {
		mix_frame(&ch1, channel2, iMixSamplesData);
	}
    
	adjust_out_level(&ch1, iMixSamplesData, mix/*sMixSamplesData*/);
	//memcpy(mix, sMixSamplesData, (kSamplesPerFrame << 1));
}

int  get_level(stAudioMixLevel* plevel, const short* frame_buffer, int samples_per_frame)
{
	int level = 0, j = 0;
	for (; j < samples_per_frame; ++ j)
		level += ((frame_buffer[j] > 0) ? frame_buffer[j] : -frame_buffer[j]);
	level /= samples_per_frame;

	// convert level to 8bit complement ulaw
	plevel->level   = audio_mix_linear2ulaw(level) ^ 0xff;
	plevel->mix_adj = AUDIO_NORMAL_LEVEL;

	return level;
}

void mix_frame(stAudioMixLevel* plevel, const short* mix_in_buffer, int* mix_out_buffer)
{
	int k = 0, tmp_adj = 0;
	for (; k < kSamplesPerFrame; ++ k) {
		mix_out_buffer[k] += mix_in_buffer[k];

		if (AUDIO_IS_OVERFLOW(mix_out_buffer[k])) {
			tmp_adj = (AUDIO_MAX_LEVEL << 7) / mix_out_buffer[k];
			if (tmp_adj < 0)
				tmp_adj = -tmp_adj;
			if (tmp_adj < plevel->mix_adj)
				plevel->mix_adj = tmp_adj;
		} // if any overflow in the mixed signals
	}	  // looping mixing singals
}

 /* If there are sources in the mix buffer, convert the mixed samples
  * from 32bit to 16bit in the mixed samples itself. This is possible 
  * because mixed sample is 32bit.
  *
  * In addition to this process, if we need to change the level of
  * TX signal, we adjust is here too.
  */

/* Calculate signal level and adjust the signal when needed. 
 * Two adjustments performed at once: 
 * 1. user setting adjustment (tx_adj_level). 
 * 2. automatic adjustment of overflowed mixed buffer (mix_adj).
 */

/* Apply simple AGC to the mix_adj, the automatic adjust, to avoid 
 * dramatic change in the level thus causing noise because the signal 
 * is now not aligned with the signal from the previous frame.
 */
int  adjust_out_level(stAudioMixLevel* plevel, const int* mix_in_buffer, short* mix_out_buffer)
{
	int adj_level = 0, tx_level = 0, m = 0;
	int temp = 0;
	
	AUDIO_SIMPLE_AGC(plevel->last_mix_adj, plevel->mix_adj);
	plevel->last_mix_adj = plevel->mix_adj;
	adj_level = plevel->tx_adj_level * plevel->mix_adj;
	adj_level >>= 7;

	for (; m < kSamplesPerFrame; ++ m) {
		temp = mix_in_buffer[m];

		// adjust the level
		if (adj_level != AUDIO_NORMAL_LEVEL)
			temp = (temp * adj_level) >> 7;

		// clip the singal if it is too loud
		if (temp > AUDIO_MAX_LEVEL)
			temp = AUDIO_MAX_LEVEL;
		else if (temp < AUDIO_MIN_LEVEL)
			temp = AUDIO_MIN_LEVEL;

		// put back in the buffer
		mix_out_buffer[m] = (short)temp;

		tx_level += ((mix_out_buffer[m] >= 0) ? mix_out_buffer[m] : -mix_out_buffer[m]);		
	}

	tx_level /= kSamplesPerFrame;

	/* Convert level to 8bit complement ulaw */
	tx_level = audio_mix_linear2ulaw(tx_level) ^ 0xff;

	plevel->tx_level = tx_level;

	return plevel->tx_level;
}



