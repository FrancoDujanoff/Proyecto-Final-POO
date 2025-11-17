/*
* picosha2: 2014-02-13 (public domain)
*
* - C++11 header-only SHA256 implementation
* - non-boost alternative to boost::uuid::sha1
* - compatible with RFC 6234 specified test vectors
* - another stream-like interface
*/
#ifndef PICOSHA2_H
#define PICOSHA2_H
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <iomanip>
#include <cassert>

namespace picosha2
{
    typedef unsigned char byte_t;
    typedef unsigned int word_t;

    namespace detail
    {
        inline byte_t ror(byte_t x, std::size_t n){ assert(n < 8); return (x >> n) | (x << (8 - n)); }
        inline word_t ror(word_t x, std::size_t n){ assert(n < 32); return (x >> n) | (x << (32 - n)); }
        inline word_t shr(word_t x, std::size_t n){ assert(n < 32); return x >> n; }
        inline word_t ch(word_t x, word_t y, word_t z){ return (x & y) ^ ((~x) & z); }
        inline word_t maj(word_t x, word_t y, word_t z){ return (x & y) ^ (x & z) ^ (y & z); }
        inline word_t rotr(word_t x, std::size_t n){ return (x >> n) | (x << (32-n)); }
        inline word_t sigma_0(word_t x){ return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22); }
        inline word_t sigma_1(word_t x){ return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25); }
        inline word_t ssigma_0(word_t x){ return rotr(x, 7) ^ rotr(x, 18) ^ shr(x, 3); }
        inline word_t ssigma_1(word_t x){ return rotr(x, 17) ^ rotr(x, 19) ^ shr(x, 10); }
        
        template<typename InIter>
        inline word_t pack(InIter first, InIter last){
            word_t res = 0;
            for(std::size_t i = 0; i < 4; ++i) res = (res << 8) | static_cast<word_t>(*first++);
            return res;
        }

        template<typename OutIter>
        inline void unpack(word_t w, OutIter first, OutIter last){
            *first++ = static_cast<byte_t>((w >> 24) & 0xff);
            *first++ = static_cast<byte_t>((w >> 16) & 0xff);
            *first++ = static_cast<byte_t>((w >> 8) & 0xff);
            *first++ = static_cast<byte_t>(w & 0xff);
        }

        static const word_t k[] = {
            0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
            0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
            0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
            0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
            0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
            0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
            0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
            0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
            0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
            0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
            0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
        };

        static const word_t h0[] = {
            0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
            0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
        };

        class context {
        public:
            context() : buffer_size_(0), total_bytes_(0) { std::copy(h0, h0+8, h_); }
            
            void add(byte_t b) {
                buffer_[buffer_size_++] = b;
                if(buffer_size_ == 64) process_block();
            }

            // --- CORRECCIÓN: Método Template para soportar Iteradores ---
            template<typename InIter>
            void add(InIter first, InIter last) {
                while (first != last) {
                    add(static_cast<byte_t>(*first));
                    ++first;
                }
            }
            // ----------------------------------------------------------

            void add(const byte_t* first, const byte_t* last) {
                std::size_t n = std::distance(first, last);
                total_bytes_ += n;
                std::size_t buffer_space = 64 - buffer_size_;
                if(buffer_size_ > 0 && n >= buffer_space) {
                    std::copy(first, first+buffer_space, buffer_.begin()+buffer_size_);
                    process_block();
                    first += buffer_space; n -= buffer_space;
                }
                while(n >= 64) {
                    std::copy(first, first+64, buffer_.begin());
                    process_block();
                    first += 64; n -= 64;
                }
                if(n > 0) {
                    std::copy(first, first+n, buffer_.begin());
                    buffer_size_ = n;
                }
            }
            void finish() {
                total_bytes_ += buffer_size_;
                buffer_[buffer_size_++] = 0x80;
                if(buffer_size_ > 56) {
                    std::fill(buffer_.begin()+buffer_size_, buffer_.end(), 0x00);
                    process_block();
                    std::fill(buffer_.begin(), buffer_.begin()+56, 0x00);
                } else {
                    std::fill(buffer_.begin()+buffer_size_, buffer_.begin()+56, 0x00);
                }
                unsigned long long total_bits = total_bytes_ * 8;
                for(std::size_t i = 0; i < 8; ++i) {
                    buffer_[63-i] = static_cast<byte_t>(total_bits & 0xff);
                    total_bits >>= 8;
                }
                process_block();
            }
            template<typename OutIter>
            void get_hash(OutIter first, OutIter last) const {
                for(std::size_t i = 0; i < 8; ++i) {
                    unpack(h_[i], first, first+4); first += 4;
                }
            }
        private:
            void process_block() {
                word_t w[64];
                std::vector<byte_t>::iterator first = buffer_.begin();
                for(std::size_t i = 0; i < 16; ++i) { w[i] = pack(first, first+4); first += 4; }
                for(std::size_t i = 16; i < 64; ++i) w[i] = ssigma_1(w[i-2]) + w[i-7] + ssigma_0(w[i-15]) + w[i-16];
                word_t a=h_[0], b=h_[1], c=h_[2], d=h_[3], e=h_[4], f=h_[5], g=h_[6], h=h_[7];
                for(std::size_t i = 0; i < 64; ++i) {
                    word_t t1 = h + sigma_1(e) + ch(e, f, g) + k[i] + w[i];
                    word_t t2 = sigma_0(a) + maj(a, b, c);
                    h=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
                }
                h_[0]+=a; h_[1]+=b; h_[2]+=c; h_[3]+=d; h_[4]+=e; h_[5]+=f; h_[6]+=g; h_[7]+=h;
                buffer_size_ = 0;
            }
            word_t h_[8];
            std::vector<byte_t> buffer_ = std::vector<byte_t>(64);
            std::size_t buffer_size_;
            unsigned long long total_bytes_;
        };
    } // detail

    template<typename InIter, typename OutIter>
    inline void hash256(InIter first, InIter last, OutIter dst_first, OutIter dst_last) {
        detail::context ctx;
        ctx.add(first, last); // Ahora esto funcionará con cualquier iterador
        ctx.finish();
        ctx.get_hash(dst_first, dst_last);
    }

    inline std::string hash256_hex_string(const std::vector<byte_t>& src) {
        std::vector<byte_t> hash(32);
        hash256(src.begin(), src.end(), hash.begin(), hash.end());
        std::stringstream ss;
        for(std::size_t i = 0; i < hash.size(); ++i)
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
        return ss.str();
    }
} 
#endif