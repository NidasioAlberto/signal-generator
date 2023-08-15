/***************************************************************************
 *   Copyright (C) 2023 by Alberto Nidasio                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   As a special exception, if other files instantiate templates or use   *
 *   macros or inline functions from this file, or you compile this file   *
 *   and link it with other works to produce a work based on this file,    *
 *   this file does not by itself cause the resulting work to be covered   *
 *   by the GNU General Public License. However the source code for this   *
 *   file must still be made available in accordance with the GNU General  *
 *   Public License. This exception does not invalidate any other reasons  *
 *   why a work based on this file might be covered by the GNU General     *
 *   Public License.                                                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

#include <assert.h>
#include <math.h>
#include <stdint.h>

struct RGB {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t white;

    RGB() : red(0), green(0), blue(0), white(0) {}

    RGB(uint8_t red, uint8_t green, uint8_t blue, uint8_t white = 0)
        : red(red), green(green), blue(blue), white(white) {}
};

struct HSV {
    uint16_t hue;
    float saturation;
    float value;

    HSV(uint16_t hue = 0, float saturation = 1, float value = 1)
        : hue(hue), saturation(saturation), value(value) {
        assert(hue <= 360);
        assert(saturation >= 0);
        assert(saturation <= 1);
        assert(value >= 0);
        assert(value <= 1);
    }

    operator RGB() const {
        // Chroma
        float C = saturation * value;

        float H_prime = hue / 60.0f;
        float X = C * (1 - abs(fmod(H_prime, 2) - 1));

        float red = 0;
        float green = 0;
        float blue = 0;

        switch (static_cast<int>(H_prime) % 6) {
            case 0:
                red = C;
                green = X;
                break;
            case 1:
                red = X;
                green = C;
                break;
            case 2:
                green = C;
                blue = X;
                break;
            case 3:
                green = X;
                blue = C;
                break;
            case 4:
                red = X;
                blue = C;
                break;
            case 5:
                red = C;
                blue = X;
                break;
            default:
                assert(false);
        }

        float m = value - C;
        red += m;
        green += m;
        blue += m;

        return RGB(red * 255, green * 255, blue * 255);
    }
};