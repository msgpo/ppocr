#include "alternation.hpp"
#include "image/image.hpp"

#include <ostream>
#include <istream>
#include <cassert>

#ifdef DEBUG_ALTERNATION
#include <iostream>
#define MAKE_SEQUENCE_ALTERNATION(name, img, x, y, B)                                          \
    (void(std::cout << name ":[" << img.bounds() << "][" << Index x, y << "] + [" << B << "]\n"), \
    make_alternations(img, x, y, B))
#else
#define MAKE_SEQUENCE_ALTERNATION(name, img, x, y, B) make_alternations(img, x, y, B)
#endif

namespace strategies
{

template<class T>
alternations::sequence_type
make_alternations(const Image & img, Index const & idx, T const & bounds)
{
    alternations::sequence_type ret;

    auto range = hrange(img, idx, bounds);
    auto it = range.begin();
    auto last = range.end();

    ret.push_back(*it);
    while (rng::next_alternation(it, last)) {
        ret.push_back(*it);
    }

    return ret;
}

alternations::alternations(const Image& img, const Image& img90)
{
    auto it = seq_alternations.begin();

    {
        Bounds const & bnd = img.bounds();

        if (bnd.h() < 2) {
            ++it;
        } else {
            *it++ = MAKE_SEQUENCE_ALTERNATION("Hl1", img, {0, (bnd.h()-2)/3}, bnd.w());
        }
        *it++ = MAKE_SEQUENCE_ALTERNATION("Hl2", img, {0, (bnd.h()*2-1)/3}, bnd.w());
        Bounds const bnd_mask(bnd.w(), bnd.h()/3);
        *it++ = MAKE_SEQUENCE_ALTERNATION("Hm1", img, {0, 0}, bnd_mask);
        *it++ = MAKE_SEQUENCE_ALTERNATION("Hm2", img, {0, bnd.h()-bnd_mask.h()}, bnd_mask);
    }

    {
        Bounds const & bnd = img90.bounds();

        *it++ = MAKE_SEQUENCE_ALTERNATION("Vl1", img90, {0, (bnd.h()-1)/2}, bnd.w());
        Bounds const bnd_mask(bnd.w(), bnd.h()/3);
        *it++ = MAKE_SEQUENCE_ALTERNATION("Vm1", img90, {0, bnd.h()-bnd_mask.h()}, bnd_mask);
        *it++ = MAKE_SEQUENCE_ALTERNATION("Vm2", img90, {0, 0}, bnd_mask);
    }

    assert(it == seq_alternations.end());
}

unsigned alternations::relationship(const alternations& other) const
{
    auto it = this->seq_alternations.begin();
    unsigned nb_equal = 0;
    for (auto const & x : other.seq_alternations) {
        if (x == *it) {
            ++nb_equal;
        }
        ++it;
    }
    return nb_equal * 100 / this->seq_alternations.size();
}

std::ostream & operator<<(std::ostream & os, alternations const & seq_alternations)
{
    for (alternations::sequence_type const & alternation : seq_alternations) {
        os << alternation.size();
        for (auto x : alternation) {
            os << ' ' << x;
        }
        os << ' ';
    }
    return os;
}

std::istream & operator>>(std::istream & is, alternations & seq_alternations)
{
    size_t sz;
    bool val;
    for (auto & alternation : seq_alternations.seq_alternations) {
        is >> sz;
        alternation.resize(sz);
        for (auto && x : alternation) {
            is >> val;
            x = val;
        }
    }
    return is;
}

}
