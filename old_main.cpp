#include <algorithm>
#include <iostream>
#include <vector>
#include <array>

#include <cassert>
#include <cstring>

#include <stdexcept> // fix range.hpp
#include <falcon/range/range.hpp>
#include <falcon/iostreams/get_file_contents.hpp>

#include "mln/image/image2d.hh"
#include "mln/io/ppm/load.hh"
#include "rgb8.hpp"

using std::size_t;

using component_type = char;

void print_image(std::ostream & os, component_type const * cs, size_t w, size_t h)
{
    os.fill(':');
    os.width(w+3);
    os << ":\n";
    while (h--) {
        os << ':';
        os.write(cs, w);
        os << ":\n";
        cs += w;
    }
    os.width(w+3);
    os << ":\n";
}

using vcomponent_type = std::vector<component_type>;

constexpr const component_type cc = ' ';
constexpr const component_type ncc = 'o';

bool isc(component_type c)
{ return c == ' '; }


struct box_type {
    size_t x;
    size_t y;
    size_t w;
    size_t h;

    explicit operator bool() const noexcept{
        return w && h;
    }
};

std::ostream & operator<<(std::ostream & os, box_type const & box) {
    return os << "box(" << box.x << ", " << box.y << ", " << box.w << ", " << box.h << ")";
}

bool horizontal_empty(component_type const * d, size_t w) {
    for (auto e = d+w; d != e; ++d) {
        if (isc(*d)) {
            return false;
        }
    }
    return true;
};
bool vertical_empty(component_type const * d, size_t w, size_t h) {
    for (auto e = d+w*h; d != e; d += w) {
        if (isc(*d)) {
            return false;
        }
    }
    return true;
};

box_type make_box_character(component_type const * cs, box_type const & cbox) {

    box_type box = cbox;

    auto d = cs + box.y*cbox.w + box.x;
    for (; box.x < cbox.w; ++box.x) {
        if (!vertical_empty(d, cbox.w, box.h)) {
            break;
        }
        ++d;
    }
    box.w = box.x;
    while (++box.w < cbox.w) {
        if (vertical_empty(d, cbox.w, box.h)) {
            break;
        }
        ++d;
    }
    box.w -= box.x + 1;

    d = cs + box.y*cbox.w + box.x;
    for (; box.y < cbox.h; ++box.y) {
        if (!horizontal_empty(d, box.w)) {
            break;
        }
        d += cbox.w;
    }
    d = cs + box.h*cbox.w + box.x;
    while (--box.h > box.y) {
        d -= cbox.w;
        if (!horizontal_empty(d, box.w)) {
            break;
        }
    }
    box.h -= box.y;

    ++box.h;
    return box;
}

vcomponent_type copy_from_box(box_type const & box, component_type const * cs, size_t w, size_t /*h*/) {
    vcomponent_type v(box.w*box.h);
    auto it = v.begin();
    cs += box.x + box.y * w;
    for (size_t y = 0; y < box.h; ++y) {
        it = std::copy(cs, cs + box.w, it);
        cs += w;
    }
    return v;
}


using vlight_type = std::vector<bool>;

vlight_type horizontal_light(vcomponent_type::const_iterator it, size_t w, size_t /*h*/) {
    vlight_type v;
    auto e = it + w;
    if (it != e) {
        v.push_back(isc(*it));
        ++it;
        for (; it != e; ++it) {
            if (isc(*it) != v.back()) {
                v.push_back(!v.back());
            }
        }
    }
    return v;
}

vlight_type vertical_light(vcomponent_type::const_iterator it, size_t w, size_t h) {
    vlight_type v;
    auto e = it + h * w;
    if (it != e) {
        v.push_back(isc(*it));
        it += w;
        for (; it != e; it += w) {
            if (isc(*it) != v.back()) {
                v.push_back(!v.back());
            }
        }
    }
    return v;
}

vlight_type horizontal_mask(vcomponent_type::const_iterator it, size_t hit, size_t w, size_t /*h*/) {
    vlight_type v;
    auto e = it + w;
    if (it != e) {
        v.push_back(!vertical_empty(it.base(), w, hit));
        ++it;
        for (; it != e; ++it) {
            if (vertical_empty(it.base(), w, hit) == v.back()) {
                v.push_back(!v.back());
            }
        }
    }
    return v;
}

vlight_type vertical_mask(vcomponent_type::const_iterator it, size_t wit, size_t w, size_t h) {
    vlight_type v;
    auto e = it + h * w;
    if (it != e) {
        v.push_back(!horizontal_empty(it.base(), wit));
        it += w;
        for (; it != e; it += w) {
            if (horizontal_empty(it.base(), wit) != v.back()) {
                v.push_back(!v.back());
            }
        }
    }
    return v;
}


void print_ligh(std::ostream & os, vlight_type const & v)
{
    for (auto && b : v) {
        os << (b  ? '.' : 'x');
    }
}


int main(/*int argc, char **argv*/)
{
    auto scomponents = falcon::iostreams::get_file_contents("/tmp/l");
    const size_t h = 15;
    const size_t w = scomponents.size() / h;
    std::cout << scomponents.size() << std::endl;
    assert(scomponents.size() == w*h);

//     const component_type components[]{
//         "oooooooooooooooooo"
//         "oooooooooooooooooo"
//         "oooooooooooooooooo"
//         "oooooooooooooooooo"
//         "oooooooooooooooooo"
//         "oooooooooooooooooo"
//         "oooooooooooooooooo"
//         "oooooooooooooooooo"
//         "oooooooooooooooooo"
//         "ooooooo   oooooooo"
//         "oooo        oooooo"
//         "ooo  oooooo   oooo"
//         "oooooooooooo  oooo"
//         "oooooooooooo  oooo"
//         "ooooooo       oooo"
//         "oooo          oooo"
//         "ooo   oooooo  oooo"
//         "oo   ooooooo  oooo"
//         "oo  oooooooo  oooo"
//         "ooo  oooooo o oooo"
//         "ooo        oo  ooo"
//         "ooooo    ooooo  oo"
//         "oooooooooooooooooo"
//         "oooooooooooooooooo"
//         "oooooooooooooooooo"
//         "oooooooooooooooooo"
//         "oooooooooooooooooo"
//         "oooooooooooooooooo"
//     };
//     const size_t w = 18;
//     const size_t h = 28;
//     static_assert(sizeof(components) == w*h + 1, "");


//     print_image(std::cout, scomponents.data(), w, h);
//     endl(std::cout);

    box_type box{0, 0, w, h};

    struct Def {
        std::array<vlight_type, 7> v;
        vcomponent_type c;
        box_type box;

        bool operator<(Def const & other) const {
            return v < other.v;
        }
        bool operator == (Def const & other) const {
            return v == other.v;
        }
    };
    std::vector<Def> all;

    while (1) {
        auto const cbox = make_box_character(scomponents.data(), box);
        if (!cbox) {
            break;
        }
        std::cout << cbox << std::endl;

        auto vbox = copy_from_box(cbox, scomponents.data(), w, h);

        print_image(std::cout, vbox.data(), cbox.w, cbox.h);

        auto vlight1 = (cbox.h < 2)
          ? vlight_type()
          : horizontal_light(vbox.cbegin() + (cbox.h-2)/3*cbox.w,               cbox.w, cbox.h);
        auto vlight2 = horizontal_light(vbox.cbegin() + (cbox.h*2-1)/3*cbox.w,  cbox.w, cbox.h);
        auto vlight3 = vertical_light(vbox.cbegin() + (cbox.w/2),               cbox.w, cbox.h);

        auto vmask1 = horizontal_mask(vbox.cbegin(),                        cbox.h/3, cbox.w, cbox.h);
        auto vmask2 = horizontal_mask(vbox.cend() - (cbox.h/3) * cbox.w,    cbox.h/3, cbox.w, cbox.h);
        auto vmask3 = vertical_mask(vbox.cbegin(),                          cbox.w/3, cbox.w, cbox.h);
        auto vmask4 = vertical_mask(vbox.cbegin() + (cbox.w/3*2),           cbox.w/3, cbox.w, cbox.h);

        print_ligh(std::cout, vlight1); endl(std::cout);
        print_ligh(std::cout, vlight2); endl(std::cout);
        print_ligh(std::cout, vlight3); endl(std::cout);
        std::cout << "--------\n";
        print_ligh(std::cout, vmask1); endl(std::cout);
        print_ligh(std::cout, vmask2); endl(std::cout);
        print_ligh(std::cout, vmask2); endl(std::cout);
        print_ligh(std::cout, vmask3); endl(std::cout);
        std::cout << "--------\n";

        all.push_back({{{
            std::move(vlight1)
          , std::move(vlight2)
          , std::move(vlight3)
          , std::move(vmask1)
          , std::move(vmask2)
          , std::move(vmask3)
          , std::move(vmask4)
        }}, std::move(vbox), cbox});

        box.x = cbox.x+cbox.w;
    }

    std::cout << " ## all.size(): " << (all.size()) << std::endl;
    std::sort(all.begin(), all.end());
    if (all.size() > 1)
    {
        auto ref = all.front();
        for (auto & x : falcon::range(all.begin()+1, all.end())) {
            if (x == ref) {
                print_image(std::cout, ref.c.data(), ref.box.w, ref.box.h);
                print_image(std::cout, x.c.data(), x.box.w, x.box.h);
                std::cout << "\n";
            }
            ref = x;
        }
    }
    all.erase(std::unique(all.begin(), all.end()), all.end());
    std::cout << " ## all.size(): " << (all.size()) << std::endl;

    mln::image2d<ocr::rgb8> img;
    if (!mln::io::ppm::load(img, "/tmp/21.pnm")) {
        if (errno != 0) {
            std::cerr << strerror(errno) << std::endl;
        } else {
            std::cerr << "error" << std::endl;
        }
        return 4;
    }

    vcomponent_type vimg(img.ncols() *img.nrows());
    auto it = vimg.begin();
    for (unsigned y = 0; y < img.nrows(); y++) {
        for (unsigned j = 0; j < img.ncols(); j++) {
            const auto & rgb = img.at(y, j);
//             unsigned char c
//             = ((511/*PPM_RED_WEIGHT*/   * rgb.red()   + 511)>>10)
//             + ((396/*PPM_GREEN_WEIGHT*/ * rgb.green() + 511)>>10)
//             + ((117/*PPM_BLUE_WEIGHT*/  * rgb.blue()  + 511)>>10);
//             *it++ = (c < 128) ? ' ' : 'x';
            *it++ = !(rgb.red() == 60 && rgb.green() == 64 && rgb.blue() == 72) ? ' ' : 'x';
//             *it++ = (rgb.red() == 255 && rgb.green() == 255 && rgb.blue() == 255) ? 'x' : ' ';
        }
    }

    {
    const size_t h = 15;
    const size_t w = vimg.size() / h;
    std::cout << vimg.size() << std::endl;
    assert(vimg.size() == w*h);

    box_type box{0, 0, w, h};

    size_t ok = 0;
    size_t fail = 0;

    while (1) {
        auto const cbox = make_box_character(vimg.data(), box);
        if (!cbox) {
            break;
        }
//         std::cout << cbox << std::endl;

        auto vbox = copy_from_box(cbox, vimg.data(), w, h);

//         print_image(std::cout, vbox.data(), cbox.w, cbox.h);

        auto vlight1 = (cbox.h < 2)
          ? vlight_type()
          : horizontal_light(vbox.cbegin() + (cbox.h-2)/3*cbox.w,               cbox.w, cbox.h);
        auto vlight2 = horizontal_light(vbox.cbegin() + (cbox.h*2-1)/3*cbox.w,  cbox.w, cbox.h);
        auto vlight3 = vertical_light(vbox.cbegin() + (cbox.w/2),               cbox.w, cbox.h);

        auto vmask1 = horizontal_mask(vbox.cbegin(),                        cbox.h/3, cbox.w, cbox.h);
        auto vmask2 = horizontal_mask(vbox.cend() - (cbox.h/3) * cbox.w,    cbox.h/3, cbox.w, cbox.h);
        auto vmask3 = vertical_mask(vbox.cbegin(),                          cbox.w/3, cbox.w, cbox.h);
        auto vmask4 = vertical_mask(vbox.cbegin() + (cbox.w/3*2),           cbox.w/3, cbox.w, cbox.h);

//         print_ligh(std::cout, vlight1); endl(std::cout);
//         print_ligh(std::cout, vlight2); endl(std::cout);
//         print_ligh(std::cout, vlight3); endl(std::cout);
//         std::cout << "--------\n";
//         print_ligh(std::cout, vmask1); endl(std::cout);
//         print_ligh(std::cout, vmask2); endl(std::cout);
//         print_ligh(std::cout, vmask2); endl(std::cout);
//         print_ligh(std::cout, vmask3); endl(std::cout);
//         std::cout << "--------\n";

        Def def = {{{
            std::move(vlight1)
          , std::move(vlight2)
          , std::move(vlight3)
          , std::move(vmask1)
          , std::move(vmask2)
          , std::move(vmask3)
          , std::move(vmask4)
        }}, std::move(vbox), cbox};

        auto p = std::lower_bound(all.begin(), all.end(), def);
        if (p != all.end() && *p == def) {
            std::cout << "ok:\n";
            print_image(std::cout, p->c.data(), p->box.w, p->box.h);
            ++ok;
        }
        else {
            std::cout << "fail:\n";
            ++fail;
//             print_image(std::cout, def.c.data(), def.box.w, def.box.h);
        }

        box.x = cbox.x+cbox.w;
    }
        std::cout << " ## ok: " << (ok) << std::endl;
        std::cout << " ## fail: " << (fail) << std::endl;
    }

    return 0;
}
