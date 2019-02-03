/*
    epple2

    Copyright © 2008, 2019, Christopher Alan Mosher, Shelton, CT, USA. <cmosher01@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY, without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "memory.h"
#include <vector>
#include <algorithm>
#include <istream>
#include <cstdlib>
#include <cstddef>
#include "raminitializer.h"



/*
 * If any RAM IC sockets are empty, set the corresponding bits to 1 most of the time.
 * But set to 0 instead, with probability 1 in 137 (a rough estimate obtained empirically)
 */
static std::uint8_t randomize_missing_bits(std::uint8_t v, const std::uint8_t bits) {
    std::uint8_t bit = 1u;
    for (std::uint_fast8_t i = 0; i < 8; ++i) {
        if (bits & bit) {
            double r = static_cast<double>(std::rand())/RAND_MAX;
            if (r < 1.0/137.0) {
                v &= ~bit;
            } else {
                v |= bit;
            }
        }
        bit <<= 1;
    }
    return v;
}



Memory::Memory(const size_t n):
    bytes(n),
    clear_value(0u),
    missing_bits(0u) {
}



void Memory::clear() {
    std::fill(this->bytes.begin(), this->bytes.end(), this->clear_value);
}

void Memory::init() {
    RAMInitializer initRam(*this);
    initRam.init();
}

void Memory::load(const std::uint16_t base, std::istream& in) {
    in.read(reinterpret_cast<char*>(&this->bytes[base]), static_cast<ptrdiff_t>(this->bytes.size()-base));
}



void Memory::powerOn() {
    init();
}

void Memory::powerOff() {
    clear();
}

size_t Memory::size() const {
    return this->bytes.size();
}

std::uint8_t Memory::read(const std::uint16_t address) const {
    std::uint8_t v = this->bytes[address];
    if (this->missing_bits) {
        v = randomize_missing_bits(v, this->missing_bits);
    }
    return v;
}

void Memory::write(const std::uint16_t address, const std::uint8_t data) {
    this->bytes[address] = data;
}
