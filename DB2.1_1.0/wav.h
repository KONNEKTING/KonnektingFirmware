#ifndef __WAV_HH__
#define __WAV_HH__

typedef struct riff_header {
	char RIFF_marker[4];
	uint32_t file_size;
	char filetype_header[4];
} riff_header;

typedef struct wav_header {
	char WAV_marker[4];
	uint32_t data_header_length;
	uint16_t format_type;
	uint16_t number_of_channels;
	uint32_t sample_rate;
	uint32_t bytes_per_second;
	uint16_t bytes_per_frame;
	uint16_t bits_per_sample;
} wav_header;

typedef struct data_header {
	char DATA_marker[4];
	uint32_t data_length;
} data_header;

#define RIFF_HEADER_SIZE sizeof(riff_header)
#define WAV_HEADER_SIZE sizeof(wav_header)
#define DATA_HEADER_SIZE sizeof(data_header)

#endif /* __WAV_HH__ */
