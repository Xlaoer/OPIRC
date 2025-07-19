// sserangecoder.h
// SSE 4.1 Interleaved Range Coding example with an 8-bit alphabet, Richard Geldreich, Jr., public domain (see full text at unlicense.org)
#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <vector>
#include <assert.h>
#include <memory.h>

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#include <smmintrin.h>
extern uint8_t* globalPtr;  //声明全局指针变量

#ifndef _MSC_VER
#define sser_forceinline __attribute__((always_inline))
#else
#define sser_forceinline __forceinline
#endif

namespace sserangecoder
{
	typedef std::vector<uint8_t> uint8_vec;
	typedef std::vector<uint32_t> uint32_vec;

	template <typename S> inline S clamp(S value, S low, S high) { return (value < low) ? low : ((value > high) ? high : value); }
	template <typename T> inline void clear_obj(T& obj) { memset(&obj, 0, sizeof(obj)); }
	
	const uint32_t cRangeCodecMinSyms = 2, cRangeCodecMaxSyms = 256;
	const uint32_t cRangeCodecMinLen = 0x00010000U, cRangeCodecMaxLen = 0x00FFFFFFU;
	const uint32_t cRangeCodecProbBits = 12;
	const uint32_t cRangeCodecProbScale = 1 << cRangeCodecProbBits;

	const uint32_t LANES = 16;
	const uint32_t LANE_MASK = LANES - 1;

	static uint32_t g_num_bytes[256];
	static __m128i g_shift_shuf[256];
	static __m128i g_dist_shuf[256];
	static __m128i g_byte_shuffle_mask;

	// Important: vrange_init() MUST be called sometime before utilizing the encoder or decoder.
	void vrange_init();
	
	// Scalar range encoder
	class range_enc
	{
	public:
		range_enc() {
			m_arith_base = 0;
			m_arith_length = cRangeCodecMaxLen;
			m_buf.resize(0);
			m_buf.reserve(4096);
		}

		inline void enc_val(uint32_t low_prob, uint32_t high_prob)
		{
			assert((low_prob < high_prob) && (high_prob <= cRangeCodecProbScale));
			assert((high_prob - low_prob) < cRangeCodecProbScale);

			uint32_t l = low_prob * (m_arith_length >> cRangeCodecProbBits);
			uint32_t h = high_prob * (m_arith_length >> cRangeCodecProbBits);

			uint32_t orig_base = m_arith_base;

			m_arith_base = (m_arith_base + l) & cRangeCodecMaxLen;
			m_arith_length = h - l;

			if (orig_base > m_arith_base)
				propagate_carry();

			if (m_arith_length < cRangeCodecMinLen) 
				renorm_enc_interval();

		}

		inline void enc_val_interleave(uint32_t low_prob, uint32_t high_prob, int& d)
		{
			assert((low_prob < high_prob) && (high_prob <= cRangeCodecProbScale));
			assert((high_prob - low_prob) < cRangeCodecProbScale);

			uint32_t l = low_prob * (m_arith_length >> cRangeCodecProbBits);
			uint32_t h = high_prob * (m_arith_length >> cRangeCodecProbBits);

			uint32_t orig_base = m_arith_base;

			m_arith_base = (m_arith_base + l) & cRangeCodecMaxLen;
			m_arith_length = h - l;

			if (orig_base > m_arith_base)
				propagate_carry_interleave (d);

			if (m_arith_length < cRangeCodecMinLen)
				renorm_enc_interval_interleave(d);

		}

		void flush();
		void flush_interleave(int& d);
		
		const uint8_vec& get_buf() const { return m_buf;}
		uint8_vec& get_buf() { return m_buf; }

		uint32_t m_arith_base, m_arith_length;
		uint8_vec m_buf;

		inline void propagate_carry()//如果buff中最新值为0xFF，将其重置为0；否则将其值加1.
		{
			if (!m_buf.size())
				return;

			size_t index = m_buf.size() - 1;

			for (; ; )
			{
				uint8_t tmp= m_buf[index];
				uint8_t& c = m_buf[index];

				if (c == 0xFF) {
					c = 0;
				}
				else
				{
					c++;
					break;
				}

				if (!index)
					break;

				index--;
			}
		}

		inline void propagate_carry_interleave(int& d)//如果buff中最新值为0xFF，将其重置为0；否则将其值加1.
		{
			int tmp = d;
			if (tmp <0)
				return;

			uint8_t c;
			for (; ; )
			{
				//if(d>=0)
					c = globalPtr[tmp];

				if (c == 0xFF) {
					globalPtr[tmp] = 0;
				}

				else
				{
					globalPtr[tmp] += 1;
					break;
				}
	
				if (!tmp)
					break;
				tmp -=3;
			}
		}

		inline void renorm_enc_interval()
		{
			assert((m_arith_base & (~cRangeCodecMaxLen)) == 0);
			do
			{
				m_buf.push_back((uint8_t)(m_arith_base >> 16));// write the midium 8 bits of m_arith_base

				m_arith_base = (m_arith_base << 8) & cRangeCodecMaxLen;

				m_arith_length <<= 8;

			} while (m_arith_length < cRangeCodecMinLen);
		}

		inline void renorm_enc_interval_interleave(int& d)
		{
			assert((m_arith_base & (~cRangeCodecMaxLen)) == 0);
			do
			{
				// store output data
				d += 3;
				*(globalPtr +d) = (uint8_t)(m_arith_base >> 16);// write the second most significant 8 bits of m_arith_base

				m_arith_base = (m_arith_base << 8) & cRangeCodecMaxLen;
				m_arith_length <<= 8;
			} while (m_arith_length < cRangeCodecMinLen);
		}
	};

	// Scalar range decoder
	class range_dec
	{
	public:
		range_dec()
		{
			assert(cRangeCodecProbBits == 12);

			clear();
		}

		void clear()
		{
			m_arith_length = 0;
			m_arith_value = 0;
		}

		void init(const uint8_t*& pBuf)// read 24 bits as the initial value
		{
			m_arith_length = cRangeCodecMaxLen;

			m_arith_value = 0;
			m_arith_value |= (pBuf[0] << 16);

			m_arith_value |= (pBuf[1] << 8);

			m_arith_value |= pBuf[2];
			pBuf += 3;
		}

		void init_read(int& d)
		{
			m_arith_length = cRangeCodecMaxLen;

			m_arith_value = 0;
			d += 3;
			m_arith_value |= (globalPtr[d]<< 16);
			//printf("read %u\t ", pBuf[encIndex]);
		
			d += 3;
			m_arith_value |= (globalPtr[d] << 8);
			//printf("read %u\t ", pBuf[encIndex + 3 * block_decs]);
			
			d += 3;
			m_arith_value |= globalPtr[d];
			//printf("read %u\n ", pBuf[encIndex + 3 * block_decs]);
		}

		inline uint32_t dec_sym(const uint32_t* pTable, const uint8_t*& pCur_buf)
		{
			const uint32_t r = (m_arith_length >> cRangeCodecProbBits);

			uint32_t q = m_arith_value / r;

			// AND is for safety in case the input stream is corrupted, it's not stricly necessary if you know it can't be
			uint32_t encoded_val = pTable[q & (cRangeCodecProbScale - 1)];

			uint32_t sym = encoded_val & 255;

			uint32_t low_prob = (encoded_val >> 8) & (cRangeCodecProbScale - 1);
			uint32_t prob_range = (encoded_val >> (8 + 12));

			//assert(q >= low_prob && (q < (low_prob + prob_range)));

			uint32_t l = low_prob * r;

			m_arith_value -= l;
			m_arith_length = prob_range * r;

			// Reads [0,2] bytes
			while (m_arith_length < cRangeCodecMinLen)
			{
				uint32_t c = *pCur_buf++;
				m_arith_value = (m_arith_value << 8) | c;
				m_arith_length <<= 8;
			}

			return sym;
		}

		inline uint32_t dec_sym_interleave(const uint32_t* pTable, int& d)
		{
			const uint32_t r = (m_arith_length >> cRangeCodecProbBits);

			uint32_t q = m_arith_value / r;
			
			// AND is for safety in case the input stream is corrupted, it's not stricly necessary if you know it can't be
			uint32_t encoded_val = pTable[q & (cRangeCodecProbScale - 1)];

			uint32_t sym = encoded_val & 255;

			uint32_t low_prob = (encoded_val >> 8) & (cRangeCodecProbScale - 1);
			uint32_t prob_range = (encoded_val >> (8 + 12));

			//assert(q >= low_prob && (q < (low_prob + prob_range)));

			uint32_t l = low_prob * r;

			m_arith_value -= l;
			m_arith_length = prob_range * r;

			// Reads [0,2] bytes
			while (m_arith_length < cRangeCodecMinLen)
			{
				d += 3;
				m_arith_value = (m_arith_value << 8) | globalPtr[d];
				m_arith_length <<= 8;
			}
		
			return sym;
		}

		uint32_t m_arith_length, m_arith_value;
	};

	// Create lookup table for the vectorized range decoder
	void vrange_init_table(uint32_t num_syms, const uint32_vec& scaled_cum_prob, uint32_vec& table);
	
	// freq may be modified if the number of used syms was 1
	bool vrange_create_cum_probs(uint32_vec& scaled_cum_prob, uint32_vec& freq);

} // sserangecoder

