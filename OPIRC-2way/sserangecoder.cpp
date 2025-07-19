// sserangecoder.cpp
// SSE 4.1 Interleaved Range Coding example with an 8-bit alphabet, Richard Geldreich, Jr., public domain (see full text at unlicense.org)
#include "sserangecoder.h"

#ifdef _MSC_VER
#pragma warning(disable:4310) // warning C4310: cast truncates constant value
#endif

namespace sserangecoder
{
	void vrange_init()
	{
		g_byte_shuffle_mask = _mm_set_epi8((char)0x80, (char)0x80, (char)0x80, (char)0x80,
			(char)0x80, (char)0x80, (char)0x80, (char)0x80,
			(char)0x80, (char)0x80, (char)0x80, (char)0x80,
			12, 8, 4, 0);

		for (uint32_t i = 0; i < 256; i++)
		{
			uint32_t num_bytes = 0;

			for (uint32_t j = 0; j < 4; j++)
			{
				if ((i >> j) & 0x10)
					num_bytes += 2;
				else if ((i >> j) & 1)
					num_bytes++;
			}

			g_num_bytes[i] = num_bytes;

			uint8_t x[16];

			for (uint32_t j = 0; j < 4; j++)
			{
				if ((i >> j) & 0x10)
				{
					x[j * 4 + 0] = 0x80;
					x[j * 4 + 1] = 0x80;
					x[j * 4 + 2] = (uint8_t)(j * 4 + 0);
					x[j * 4 + 3] = (uint8_t)(j * 4 + 1);
				}
				else if ((i >> j) & 1)
				{
					x[j * 4 + 0] = 0x80;
					x[j * 4 + 1] = (uint8_t)(j * 4 + 0);
					x[j * 4 + 2] = (uint8_t)(j * 4 + 1);
					x[j * 4 + 3] = (uint8_t)(j * 4 + 2);
				}
				else
				{
					x[j * 4 + 0] = (uint8_t)(j * 4 + 0);
					x[j * 4 + 1] = (uint8_t)(j * 4 + 1);
					x[j * 4 + 2] = (uint8_t)(j * 4 + 2);
					x[j * 4 + 3] = (uint8_t)(j * 4 + 3);
				}
			}
			g_shift_shuf[i] = _mm_loadu_si128((__m128i *)&x);

			uint32_t src_ofs = 0;
			for (uint32_t j = 0; j < 4; j++)
			{
				if ((i >> j) & 0x10)
				{
					x[j * 4 + 0] = (uint8_t)(src_ofs + 1);
					x[j * 4 + 1] = (uint8_t)(src_ofs);
					x[j * 4 + 2] = 0x80;
					x[j * 4 + 3] = 0x80;
					src_ofs += 2;
				}
				else if ((i >> j) & 1)
				{
					x[j * 4 + 0] = (uint8_t)(src_ofs++);
					x[j * 4 + 1] = 0x80;
					x[j * 4 + 2] = 0x80;
					x[j * 4 + 3] = 0x80;
				}
				else
				{
					x[j * 4 + 0] = 0x80;
					x[j * 4 + 1] = 0x80;
					x[j * 4 + 2] = 0x80;
					x[j * 4 + 3] = 0x80;
				}
			}

			g_dist_shuf[i] = _mm_loadu_si128((__m128i *)&x);
		}
	}

	void range_enc::flush()// 使用24 bits计算精度
	{
		uint32_t orig_base = m_arith_base;
		for (int i = 0; i < 3; i++)
		{
			m_buf.push_back((uint8_t)(orig_base >> 16));
			orig_base = orig_base << 8;
		}
	}

	void range_enc::flush_interleave(int& d)// 使用24 bits计算精度
	{
		uint32_t orig_base = m_arith_base;
		for (int i = 0; i < 3; i++)
		{
			d += 2;
			globalPtr[d]= (uint8_t)(orig_base >> 16);
			orig_base = orig_base << 8;
		}
	}

	// Create lookup table for the vectorized range decoder
	void vrange_init_table(uint32_t num_syms, const uint32_vec& scaled_cum_prob, uint32_vec& table)
	{
		assert(*(const uint32_t*)&g_byte_shuffle_mask != 0);

		table.resize(cRangeCodecProbScale);
		assert(scaled_cum_prob.size() == (num_syms + 1));

		for (uint32_t sym_index = 0; sym_index < num_syms; sym_index++)
		{
			const uint32_t n = scaled_cum_prob[sym_index + 1] - scaled_cum_prob[sym_index];
			if (!n)
				continue;

			assert(scaled_cum_prob[sym_index] < cRangeCodecProbScale);
			assert((scaled_cum_prob[sym_index + 1] - scaled_cum_prob[sym_index]) < cRangeCodecProbScale);

			const uint32_t k = sym_index | (scaled_cum_prob[sym_index] << 8) | ((scaled_cum_prob[sym_index + 1] - scaled_cum_prob[sym_index]) << 20);

			uint32_t* pDst = &table[scaled_cum_prob[sym_index]];
			for (uint32_t j = 0; j < n; j++)
				*pDst++ = k;
		}
	}

	// freq may be modified if the number of used syms was 1
	bool vrange_create_cum_probs(uint32_vec& scaled_cum_prob, uint32_vec& freq)
	{
		assert(*(const uint32_t*)&g_byte_shuffle_mask != 0);

		const uint32_t num_syms = (uint32_t)freq.size();
		assert((num_syms >= cRangeCodecMinSyms) && (num_syms <= cRangeCodecMaxSyms));

		if ((num_syms < cRangeCodecMinSyms) || (num_syms > cRangeCodecMaxSyms))
			return false;

		uint64_t total_freq = 0;
		uint32_t total_used_syms = 0;
		for (uint32_t i = 0; i < num_syms; i++)
		{
			total_freq += freq[i];
			if (freq[i])
				total_used_syms++;
		}

		assert(total_used_syms >= 1);
		if (!total_used_syms)
			return false;

		if (total_used_syms == 1)
		{
			for (uint32_t i = 0; i < num_syms; i++)
			{
				if (!freq[i])
				{
					freq[i]++;
					total_freq++;
					break;
				}
			}

			total_used_syms++;
		}

		assert((total_used_syms >= 2) && (total_freq >= 2));

		scaled_cum_prob.resize(num_syms + 1);

		uint32_t sym_index_to_boost = 0, boost_amount = 0;

		uint32_t adjusted_prob_scale = cRangeCodecProbScale;
		for (; ; )
		{
			// Count how many used symbols would get truncated to a frequency of 0 
			// These symbols could cause the total frequency to be too large, because they get assigned a minimum frequency of 1 (not 0)
			uint32_t num_truncated_syms = 0;
			for (uint32_t i = 0; i < num_syms; i++)
			{
				if (freq[i])
				{
					uint32_t l = (uint32_t)(((uint64_t)freq[i] * adjusted_prob_scale) / total_freq);
					if (!l)
						num_truncated_syms++;
				}
			}

			// If no symbols get a truncated freq of 0 then our scale is good
			if (!num_truncated_syms)
				break;

			// Compute new lower scale, compensating for the # of symbols which get a boosted freq of 1
			uint32_t new_adjusted_prob_scale = cRangeCodecProbScale - num_truncated_syms;
			if (new_adjusted_prob_scale == adjusted_prob_scale)
				break;

			// The prob scale is now lower, so recount how many symbols get truncated. This can't loop forever, because num_truncated_syms can only go so high (255)
			adjusted_prob_scale = new_adjusted_prob_scale;
		}

		for (uint32_t pass = 0; pass < 2; pass++)
		{
			uint32_t most_prob_sym_freq = 0, most_prob_sym_index = 0;

			uint32_t ci = 0;
			for (uint32_t i = 0; i < num_syms; i++)
			{
				scaled_cum_prob[i] = ci;

				if (!freq[i])
					continue;

				if (freq[i] > most_prob_sym_freq)
				{
					most_prob_sym_freq = freq[i];
					most_prob_sym_index = i;
				}

				uint32_t l = (uint32_t)(((uint64_t)freq[i] * adjusted_prob_scale) / total_freq);
				l = clamp<uint32_t>(l, 1, cRangeCodecProbScale - (total_used_syms - 1));

				if ((pass) && (i == sym_index_to_boost))
					l += boost_amount;

				ci += l;
				assert(ci <= cRangeCodecProbScale);

				// shouldn't happen
				if (ci > cRangeCodecProbScale)
					return false;
			}
			scaled_cum_prob[num_syms] = cRangeCodecProbScale;
						
			if (ci == cRangeCodecProbScale)
				break;

			// shouldn't happen
			if (pass)
				return false;

			assert(!pass);

			// On first pass and the total frequency isn't cRangeCodecProbScale, so boost the freq of the max used symbol

			sym_index_to_boost = most_prob_sym_index;
			boost_amount = cRangeCodecProbScale - ci;
		}

		return true;
	}

	static sser_forceinline uint32_t read_be24(const uint8_t*& pSrc)
	{
		const uint32_t res = (pSrc[0] << 16) | (pSrc[1] << 8) | pSrc[2];
		pSrc += 3;
		return res;
	}

} // namespace sserangecoder


