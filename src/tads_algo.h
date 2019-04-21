
int tads_algo_init(int rate, int format, int channels_in, int channels_out);

int tads_algo_pump(void *input, void *output, int sample_num);

int tads_algo_close();
