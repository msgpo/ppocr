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
 *   Product name: redemption, a FLOSS RDP proxy
 *   Copyright (C) Wallix 2010-2013
 *   Author(s): Christophe Grosjean, Raphael Zhou, Jonathan Poelen, Meng Tan
 */

#ifndef REDEMPTION_WAB_9_X_CAPTURE_OCR_FONT_HPP
#define REDEMPTION_WAB_9_X_CAPTURE_OCR_FONT_HPP

#include <mln/image/image2d.hh>

#include <type_traits>


namespace ocr {

namespace {
    struct unknown_addr_ {
        static constexpr const char unknown_array[2] = "?";
    };
    constexpr decltype(unknown_addr_::unknown_array) unknown_addr_::unknown_array;
}
constexpr const char * const & unknown = unknown_addr_::unknown_array;

namespace fonts {
    struct Pixel {
        const ::mln::image2d<bool>& ima_;
        const ::mln::box2d & bbox_;
        const unsigned col_;

        Pixel(const ::mln::image2d<bool>& ima, const ::mln::box2d & bbox, unsigned ncols)
        : ima_(ima)
        , bbox_(bbox)
        , col_(ncols)
        {}

        inline
        bool operator()(unsigned pos) const noexcept
        {
            return this->ima_.at(this->bbox_.min_row() + pos / this->col_,
                                 this->bbox_.min_col() + pos % this->col_);
        }
    };

    struct Font {
        struct Replacement {
            const char * pattern;
            const char * replace;
        };

        const char * name;
        const char * (*const classify)(unsigned ncol, unsigned nrow, unsigned npix, Pixel accessor);
        const Replacement * replacements;

        unsigned min_height_char;
        unsigned max_height_char;
        unsigned whitespace_width;

        //bool latin_to_cyrillic;
    };

    namespace {
#       include "../../tools/ocr/common_classifier.hxx"
#       include "../../tools/ocr/latin_classifier.hxx"
#       include "../../tools/ocr/cyrillic_classifier.hxx"
    }

    static constexpr Font latin_fonts[] = {
#       include "../../tools/ocr/common_classifier.names.hxx"
#       include "../../tools/ocr/latin_classifier.names.hxx"
    };

    static constexpr Font cyrillic_fonts[] = {
#       include "../../tools/ocr/common_classifier.names.hxx"
#       include "../../tools/ocr/cyrillic_classifier.names.hxx"
    };

    static constexpr Font const * fonts[] = {latin_fonts, cyrillic_fonts};

    struct LocaleId {
        using type_id = unsigned;
        enum E : type_id {
            latin
          , cyrillic
          , max
        };

        explicit LocaleId(type_id id) noexcept
        : id(id)
        {}

        template<class T, class = std::enable_if<std::is_same<E, T>::value>>
        LocaleId(T id) noexcept
        : id(id)
        {}

        LocaleId & operator=(type_id id) noexcept
        {
            this->id = id;
            return *this;
        }

        operator unsigned () const noexcept
        { return this->id; }

    private:
        type_id id;
    };

    static constexpr unsigned nfonts[] = {
        sizeof(latin_fonts)/sizeof(latin_fonts[0])
      , sizeof(cyrillic_fonts)/sizeof(cyrillic_fonts[0])
    };

    namespace internal {
        template<std::size_t N>
        /*constexpr*/ inline unsigned min_height(Font const (& fonts)[N]) noexcept
        {
            unsigned ret = fonts[0].min_height_char;
            for (unsigned i = 1; i < N; ++i) {
                if (ret > fonts[i].min_height_char) {
                    ret = fonts[i].min_height_char;
                }
            }
            return ret;
        }

        template<std::size_t N>
        /*constexpr*/ inline unsigned max_height(Font const (& fonts)[N]) noexcept
        {
            unsigned ret = 0;
            for (unsigned i = 0; i < N; ++i) {
                if (ret < fonts[i].max_height_char) {
                    ret = fonts[i].max_height_char;
                }
            }
            return ret;
        }
    }

    static const/*expr*/ unsigned min_height_font[] = {
        internal::min_height(latin_fonts)
      , internal::min_height(cyrillic_fonts)
    };
    static const/*expr*/ unsigned max_height_font[] = {
        internal::max_height(latin_fonts)
      , internal::max_height(cyrillic_fonts)
    };

    inline unsigned font_id_by_name(LocaleId locale_id, const char * name) noexcept
    {
        for (unsigned i = 0; i < nfonts[locale_id]; ++i) {
            if (!strcmp(name, fonts[locale_id][i].name)) {
                return i;
            }
        }
        return -1u;
    }
}
}

#endif
