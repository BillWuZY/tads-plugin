
#include <alsa/asoundlib.h>
#include <alsa/pcm_external.h>

#include "plug_conf.h"
#include "tads_algo.h"

//#define LOCAL_PASSTHROUGH

typedef struct {
	snd_pcm_extplug_t	ext;
} snd_pcm_tads_t;


static int tads_init(snd_pcm_extplug_t *ext)
{
	SNDERR("rate %d, channels %d, format %d; s_channels %d, s_format %d\n",
		ext->rate, ext->channels, ext->format,
		ext->slave_channels, ext->slave_format);

	tads_algo_init(ext->rate, ext->format, ext->channels, ext->slave_channels);

	return 0;
}

static int tads_close(snd_pcm_extplug_t *ext)
{
	tads_algo_close();
	return 0;
}

static inline void *area_addr(const snd_pcm_channel_area_t *area,
			      snd_pcm_uframes_t offset)
{
	unsigned int bitofs = area->first + area->step * offset;
	return (char *) area->addr + bitofs / 8;
}

static int tads_passthrough(snd_pcm_extplug_t *ext,
			void *in, void *out, int sample)
{
	int bits = snd_pcm_format_width(ext->format);	
	int bytes = bits / 8;

	memcpy(out, in, sample * ext->slave_channels * bytes);

	return 0;
} 

static snd_pcm_sframes_t tads_transfer(snd_pcm_extplug_t *ext,
	     		const snd_pcm_channel_area_t *dst_areas,
	     		snd_pcm_uframes_t dst_offset,
	     		const snd_pcm_channel_area_t *src_areas,
	     		snd_pcm_uframes_t src_offset,
	     		snd_pcm_uframes_t size)
{
	void *src = area_addr(src_areas, src_offset);
	void *dst = area_addr(dst_areas, dst_offset);

#ifdef LOCAL_PASSTHROUGH
	tads_passthrough(ext, src, dst, size);	
#else
	tads_algo_pump(src, dst, size);
#endif
	return size; 
}


static void constraint_config(snd_pcm_extplug_t *ext)
{
	snd_pcm_extplug_set_param_minmax(ext, SND_PCM_EXTPLUG_HW_CHANNELS, 
					INPUT_CHANNELS_MIN, INPUT_CHANNELS_MAX);
	snd_pcm_extplug_set_slave_param_minmax(ext, SND_PCM_EXTPLUG_HW_CHANNELS, 
					OUTPUT_CHANNELS_MIN, OUTPUT_CHANNELS_MAX);

	snd_pcm_extplug_set_param_list(ext, SND_PCM_EXTPLUG_HW_FORMAT,
			sizeof(input_format_list)/sizeof(input_format_list[0]),
			input_format_list);

	snd_pcm_extplug_set_slave_param_list(ext, SND_PCM_EXTPLUG_HW_FORMAT,
			sizeof(output_format_list)/sizeof(output_format_list[0]),
			output_format_list);
}

static const snd_pcm_extplug_callback_t tads_callback = {
	.transfer = tads_transfer,
	.init = tads_init,
	.close = tads_close,
};

SND_PCM_PLUGIN_DEFINE_FUNC(tads)
{
	snd_pcm_tads_t *ts;
	snd_pcm_extplug_t *ext;
	snd_config_t *sconf = NULL;
	snd_config_iterator_t i, next;
	int err = 0;

	snd_config_for_each(i, next, conf) {
		snd_config_t *n = snd_config_iterator_entry(i);
		const char *id;
		if (snd_config_get_id(n, &id) < 0)
			continue;
		if (strcmp(id, "slave") == 0) {
			sconf = n;
			continue;
		}
	}

	if (!sconf) {
		SNDERR("No slave configuration for tads pcm");
		return -EINVAL;	
	}

	ts = calloc(1, sizeof(*ts));
	if (!ts)
		return -ENOMEM;
	
	ext = &ts->ext;	

	ext->version = SND_PCM_EXTPLUG_VERSION;
	ext->name = "TADS DSP Plugin";
	ext->callback = &tads_callback;
	ext->private_data = ts;

	err = snd_pcm_extplug_create(ext, name, root, 
					sconf, stream, mode);
	if (err < 0) {
		SNDERR("extplug create fail %d\n", err);
		free(ts);
		return err;
	}

	constraint_config(ext);

	*pcmp = ext->pcm;
	
	return err;
}

SND_PCM_PLUGIN_SYMBOL(tads);
