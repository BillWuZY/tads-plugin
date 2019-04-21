#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

int _rate;
int _format;
int _ch_in;
int _ch_out;

int tads_algo_init(int rate, int format, int channels_in, int channels_out)
{
	printf("--- %s --- \n", __func__);
	_rate = rate;
	_format = format;
	_ch_in = channels_in;
	_ch_out = channels_out;

	return 0;
}

int tads_algo_pump(void *input, void *output, int sample_num)
{
	int bits = snd_pcm_format_width(_format);	
	int bytes = bits / 8;

	memcpy(output, input, sample_num * _ch_out * bytes);
	
	return 0;
}

int tads_algo_close()
{
	printf("--- %s --- \n", __func__);
	return 0;
}
