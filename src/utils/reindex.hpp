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
*   Copyright (C) Wallix 2010-2015
*   Author(s): Jonathan Poelen
*/

#ifndef PPOCR_SRC_UTILS_REINDEX_HPP
#define PPOCR_SRC_UTILS_REINDEX_HPP

#include <utility>
#include <vector>

namespace ppocr {
namespace utils {

template<class Index, class T>
void reindex(std::vector<Index> const & indexes, std::vector<T> & cont) {
    std::vector<T> new_cont;
    new_cont.resize(cont.size());
    auto it = new_cont.begin();
    for (auto i : indexes) {
        *it = std::move(cont[i]);
        ++it;
    }
    cont = std::move(new_cont);
}

}
}

#endif
