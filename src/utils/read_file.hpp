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

#ifndef PPOCR_SRC_UTILS_READ_FILE_HPP
#define PPOCR_SRC_UTILS_READ_FILE_HPP

#include <fstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>

namespace ppocr { namespace utils {

template<class Fn>
void check_read_file(char const * filename, Fn reader) {
    std::ifstream file(filename);

    if (!file) {
        throw std::runtime_error(std::string(filename) + " : open error: " + strerror(errno));
    }

    reader(file);

    if (!file.eof()) {
        throw std::runtime_error(std::string(filename) + " : read error: " + strerror(errno));
    }
}

template<class T>
void read_file(T & x, char const * filename) {
    check_read_file(filename, [&](std::istream & file) { file >> x; });
}

template<class T>
T load_from_file(char const * filename) {
    T ret;
    check_read_file(filename, [&](std::istream & file) { file >> ret; });
    return ret;
}


template<class Fn>
void check_read_file(std::string const & filename, Fn reader) {
    check_read_file(filename.c_str(), reader);
}

template<class T>
void read_file(T & x, std::string const &filename) {
    read_file(x, filename.c_str());
}
template<class T>
T load_from_file(std::string const & filename) {
    return load_from_file<T>(filename.c_str());
}

} }

#endif
