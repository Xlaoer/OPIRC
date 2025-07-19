// SSE 4.1 Interleaved Range Coding example with an 8-bit alphabet, Richard Geldreich, Jr., public domain (see full text at unlicense.org)
// Simple test app with 3 modes (compression/decompression testing, compression, or decompression)
#include "sserangecoder.h"
#include <stdarg.h>
#include <time.h>
#include <math.h>

// The CRC-32 check is so slow it's the bottleneck in this app during decompression (using the 'd' mode command), ignoring file I/O.
// Disable if you only want to benchmark the decompressor (and file I/O) and not the slow CRC-32.
#define DECOMP_CRC32_CHECKING 1


#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef _MSC_VER
#pragma warning (disable:4127) // warning C4127: conditional expression is constant
#endif

using namespace sserangecoder;

typedef std::vector<float> float_vec;

typedef uint64_t timer_ticks;

#if defined(_WIN32)
inline void query_counter(timer_ticks* pTicks)
{
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(pTicks));
}
inline void query_counter_frequency(timer_ticks* pTicks)
{
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(pTicks));
}
#elif defined(__APPLE__)
#include <sys/time.h>
inline void query_counter(timer_ticks* pTicks)
{
	struct timeval cur_time;
	gettimeofday(&cur_time, NULL);
	*pTicks = static_cast<unsigned long long>(cur_time.tv_sec) * 1000000ULL + static_cast<unsigned long long>(cur_time.tv_usec);
}
inline void query_counter_frequency(timer_ticks* pTicks)
{
	*pTicks = 1000000;
}
#elif defined(__GNUC__)
#include <sys/timex.h>
inline void query_counter(timer_ticks* pTicks)
{
	struct timeval cur_time;
	gettimeofday(&cur_time, NULL);
	*pTicks = static_cast<unsigned long long>(cur_time.tv_sec) * 1000000ULL + static_cast<unsigned long long>(cur_time.tv_usec);
}
inline void query_counter_frequency(timer_ticks* pTicks)
{
	*pTicks = 1000000;
}
#else
#error TODO
#endif

static uint64_t get_clock()
{
	uint64_t res;
	query_counter(&res);
	return res;
}

static uint64_t get_ticks_per_sec()
{
	uint64_t res;
	query_counter_frequency(&res);
	return res;
}

static void panic(const char* pMsg, ...)
{
	fprintf(stderr, "ERROR: ");

	va_list args;
	va_start(args, pMsg);
	vfprintf(stderr, pMsg, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

static bool read_file_to_vec(const char* pFilename, uint8_vec& data)
{
	FILE* pFile = nullptr;
#ifdef _WIN32
	fopen_s(&pFile, pFilename, "rb");
#else
	pFile = fopen(pFilename, "rb");
#endif
	if (!pFile)
		return false;

	fseek(pFile, 0, SEEK_END);
#ifdef _WIN32
	int64_t filesize = _ftelli64(pFile);
#else
	int64_t filesize = ftello(pFile);
#endif
	if (filesize < 0)
	{
		fclose(pFile);
		return false;
	}
	fseek(pFile, 0, SEEK_SET);

	if (sizeof(size_t) == sizeof(uint32_t))
	{
		if (filesize > 0x70000000)
		{
			// File might be too big to load safely in one alloc
			fclose(pFile);
			return false;
		}
	}

	data.resize((size_t)filesize);

	if (filesize)
	{
		if (fread(&data[0], 1, (size_t)filesize, pFile) != (size_t)filesize)
		{
			fclose(pFile);
			return false;
		}
	}

	fclose(pFile);
	return true;
}

static bool write_data_to_file(const char* pFilename, const void* pData, size_t len)
{
	FILE* pFile = nullptr;
#ifdef _WIN32
	fopen_s(&pFile, pFilename, "wb");
#else
	pFile = fopen(pFilename, "wb");
#endif
	if (!pFile)
		return false;

	if (len)
	{
		if (fwrite(pData, 1, len, pFile) != len)
		{
			fclose(pFile);
			return false;
		}
	}

	return fclose(pFile) != EOF;
}

static void test_plain_range_coding(
	const uint8_vec &file_data, 
	const uint32_vec &scaled_cum_prob, 
	const uint32_vec &dec_table)
{
	//printf("\n-----------------Testing plain range coding!-----------------\n");

	const uint32_t file_size = (uint32_t)file_data.size();
		
	range_enc enc;

	enc.get_buf().reserve(file_size);

	const uint64_t enc_start_time = get_clock();


	for (uint32_t i = 0; i < file_size; i++)
	{
		uint32_t sym = file_data[i];
		enc.enc_val(scaled_cum_prob[sym], scaled_cum_prob[sym + 1]);
	}
	enc.flush();

	const double total_enc_time = (double)(get_clock() - enc_start_time) / (double)get_ticks_per_sec();

	printf("plain encoding  %.1f MiB/sec.\n",  ((double)file_size / total_enc_time) / (1024 * 1024));
		
	

	const uint8_t* pCur_buf = &enc.get_buf()[0];

	uint8_vec decoded_buf(file_size);
	memset(&decoded_buf[0], 0xCD, file_size);

	const uint64_t dec_start_time = get_clock();

	range_dec dec;
	dec.init(pCur_buf);// read 24 bits data as the initial value
		
	for (uint32_t i = 0; i < file_size; i++)
		decoded_buf[i] = (uint8_t)dec.dec_sym(&dec_table[0], pCur_buf);

	
	const double total_dec_time = (double)(get_clock() - dec_start_time) / (double)get_ticks_per_sec();

	printf("plain decoding  %.1f MiB/sec.\n", ((double)file_size / total_dec_time) / (1024 * 1024));
	
	printf("plain Compressed file from %zu bytes to %zu bytes\n", 
		file_data.size(), enc.get_buf().size());

	//if (memcmp(&decoded_buf[0], &file_data[0], file_data.size()) != 0)
	//	panic("Decompression failed!\n");
	//else
	//	printf("Decompression OK\n");

}

int checkConditions(bool A, bool B, bool C) {
	// 将布尔值转换为整数 (1/0)，并用加权求和表示不同条件组合
	return (A << 2) | (B << 1) | C;
}
	
uint8_t* globalPtr = nullptr;  // 定义全局指针并初始化为nullptr

int main(int argc, char** argv)
{
	vrange_init();

	const char* pSrc_filename;
	if(argc>1)
		pSrc_filename = argv[1];
	else
		pSrc_filename = "book1";


	printf("Reading file %s\n", pSrc_filename);

	uint8_vec file_data;
	if (!read_file_to_vec(pSrc_filename, file_data))
		panic("Failed reading source file!\n");

	if (!file_data.size())
		panic("File empty!\n");

	if (file_data.size() >= UINT32_MAX)
		panic("File too big!\n");

	const uint32_t file_size = (uint32_t)file_data.size();

	// count symbol frequency
	uint32_vec sym_freq(256);
	for (uint32_t i = 0; i < file_data.size(); i++)
		sym_freq[file_data[i]]++;

	//scaled symbol frequency
	uint32_vec scaled_cum_prob;
	bool status = vrange_create_cum_probs(scaled_cum_prob, sym_freq);
	if (!status)
		panic("vrange_create_cum_probs() failed!\n");

	uint32_vec dec_table;
	vrange_init_table(256, scaled_cum_prob, dec_table);

	test_plain_range_coding(file_data, scaled_cum_prob, dec_table);

	//Encoding
	int EncCnt = 3;
   //printf("\n------------Output-Interleaved Encoding starts!--------------\n");
	//printf("The number of range encoder used is %d\n", EncCnt);

	static size_t out_max_elems = file_size;
	uint8_t* output_buf = new uint8_t[out_max_elems];

	globalPtr = output_buf;

	range_enc encs0, encs1, encs2;//定义3个编码器
	encs0.get_buf().reserve(1 + (file_size / 3));//为每个编码器的输出缓冲分配空间
	encs1.get_buf().reserve(1 + (file_size / 3));
	encs2.get_buf().reserve(1 + (file_size / 3));

	int CurBlockIndex = 0;//窗口的起始块索引

	int e0 = -3, e1 = -2, e2 = -1;//编码器存放输出的起始位置

	int i = 0;
	uint32_t sym0, sym1, sym2;

	int res = 0;
	const uint64_t start_time_enc = get_clock();

	int index = file_size - 3;

	while (i < index) {
		//run encs0
		if (e0 < CurBlockIndex) {
			sym0 = file_data[i++];
			encs0.enc_val_interleave(scaled_cum_prob[sym0], scaled_cum_prob[sym0 + 1], e0);
		}

		//run encs1
		if (e1 < CurBlockIndex) {
			sym1 = file_data[i++];
			encs1.enc_val_interleave(scaled_cum_prob[sym1], scaled_cum_prob[sym1 + 1], e1);
		}

		//run encs2
		if (e2 < CurBlockIndex) {
			sym2 = file_data[i++];
			encs2.enc_val_interleave(scaled_cum_prob[sym2], scaled_cum_prob[sym2 + 1], e2);
		}

		if (e0 < CurBlockIndex || e1 < CurBlockIndex || e2 < CurBlockIndex)
			continue;
		CurBlockIndex += 3;
	}

	// Encode the last 3 symbols
	while (i < file_size) {
		//run encs0
		if (e0 < CurBlockIndex) {
			sym0 = file_data[i++];
			encs0.enc_val_interleave(scaled_cum_prob[sym0], scaled_cum_prob[sym0 + 1], e0);
		}

		//run encs1
		if (e1 < CurBlockIndex && i < file_size) {
			sym1 = file_data[i++];
			encs1.enc_val_interleave(scaled_cum_prob[sym1], scaled_cum_prob[sym1 + 1], e1);
		}

		//run encs2
		if (e2 < CurBlockIndex && i < file_size) {
			sym2 = file_data[i++];
			encs2.enc_val_interleave(scaled_cum_prob[sym2], scaled_cum_prob[sym2 + 1], e2);
		}

		if (e0 < CurBlockIndex || e1 < CurBlockIndex || e2 < CurBlockIndex)
			continue;
		CurBlockIndex += 3;
	
	}

	// Flush all encoders
	encs0.flush_interleave(e0);
	encs1.flush_interleave(e1);
	encs2.flush_interleave(e2);

	const double total_time_enc = (double)(get_clock() - start_time_enc) / (double)get_ticks_per_sec();
	printf("OPIRC-3way encoding %.1f MiB/sec.\n", 
		(file_data.size() / total_time_enc) / (1024 * 1024));

	int max_ptr = e0;
	max_ptr = e1 > max_ptr ? e1 : max_ptr;
	max_ptr = e2 > max_ptr ? e2 : max_ptr;

	

	//Decoding
	//printf("\n------------Output-Interleaved Decoding starts!--------------\n");
	
	int d0 = -3, d1 = -2, d2 = -1;//解码器读取输出的起始位置

	uint8_vec decoded_symbol(file_size);//存放解码后的符号
	memset(&decoded_symbol[0], 0xCD, file_size);

	const uint64_t dec_start_time = get_clock();

	range_dec dec0, dec1, dec2;

	//Init decoder
	dec0.init_read(d0);// read 24 bits data as the initial value
	dec1.init_read(d1);
	dec2.init_read(d2);
	
	CurBlockIndex = 9;

    i = 0;

	while (i < index) {
		//run dec0
		if (d0 < CurBlockIndex) {
			decoded_symbol[i++] = (uint8_t)dec0.dec_sym_interleave(&dec_table[0], d0);
		}

		//run dec1
		if (d1 < CurBlockIndex) {
			decoded_symbol[i++] = (uint8_t)dec1.dec_sym_interleave(&dec_table[0], d1);
		}

		//run dec2
		if (d2 < CurBlockIndex) {
			decoded_symbol[i++] = (uint8_t)dec2.dec_sym_interleave(&dec_table[0], d2);
		}

		if (d0 < CurBlockIndex || d1 < CurBlockIndex || d2 < CurBlockIndex)
			continue;
		CurBlockIndex += 3;
	}

	// Decode the last 3 symbols
	while (i < file_size) {
		//run dec0
		if (d0 < CurBlockIndex) {
			decoded_symbol[i++] = (uint8_t)dec0.dec_sym_interleave(&dec_table[0], d0);
		}

		//run dec1
		if (d1 < CurBlockIndex && i < file_size) {
			decoded_symbol[i++] = (uint8_t)dec1.dec_sym_interleave(&dec_table[0], d1);
		}

		//run dec2
		if (d2 < CurBlockIndex && i < file_size) {
			decoded_symbol[i++] = (uint8_t)dec2.dec_sym_interleave(&dec_table[0], d2);
		}

		if (d0 < CurBlockIndex || d1 < CurBlockIndex || d2 < CurBlockIndex)
			continue;
		CurBlockIndex += 3;
	}

	const double total_dec_time = (double)(get_clock() - dec_start_time) / (double)get_ticks_per_sec();

	printf("OPIRC-3way decoding  %.1f MiB/sec.\n", ((double)file_size / total_dec_time) / (1024 * 1024));

	printf("OPIRC-3way Compressed file from %zu bytes to %zu bytes\n",
		file_data.size(), max_ptr+1);
		
	if (memcmp(&decoded_symbol[0], &file_data[0], file_data.size()) != 0)
		printf("Decompression failed!\n");
	else 
		printf("Decompression OK\n");

	delete[] output_buf;
    return EXIT_SUCCESS;
}



