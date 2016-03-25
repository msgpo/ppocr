/*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*   Copyright (C) Wallix 2015
*   Author(s): Jonathan Poelen
*/

#ifndef PPOCR_STRATEGIES_UTILS_HORIZONTAL_GRAVITY_HPP
#define PPOCR_STRATEGIES_UTILS_HORIZONTAL_GRAVITY_HPP

#include "image/image.hpp"
#include "top_bottom.hpp"


namespace ppocr { namespace strategies { namespace utils {

/**
 * Multiply each pixel with the distance [\a bnd.h(), y]
 */
inline
unsigned horizontal_box_gravity(Bounds const & bnd, Pixel const * p, Pixel const * ep, bool is_top)
{
    unsigned /*long*/ g = 0;
    unsigned h = is_top ? bnd.h() : 1;
    while (p != ep) {
        for (auto epl = p + bnd.w(); p != epl; ++p) {
            if (is_pix_letter(*p)) {
                g += h;
            }
        }

        if (is_top) {
            --h;
        }
        else {
            ++h;
        }
    }
    return g;
}

inline TopBottom horizontal_gravity(const Image& img)
{
    Bounds const bnd(img.width(), img.height() / 2);
    auto p = img.data();
    auto ep = img.data({0, bnd.h()});
    unsigned const top = horizontal_box_gravity(bnd, p, ep, true);
    p = ep;
    if (img.height() & 1) {
        p += img.width();
    }
    unsigned const bottom = horizontal_box_gravity(bnd, p, img.data_end(), false);

    return {top, bottom};
}

} } }

#endif
