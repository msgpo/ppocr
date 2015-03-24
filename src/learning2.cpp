#include "image/image.hpp"
#include "factory/data_loader.hpp"
#include "box_char/make_box_character.hpp"
#include "factory/registry.hpp"
#include "factory/definition.hpp"
#include "utils/image_compare.hpp"
#include "utils/unique_sort_definition.hpp"

#include <iostream>
#include <fstream>

#include <cstring>
#include <cerrno>
#include <cassert>

static int load_file(
    std::vector<Definition> & definitions, DataLoader const & loader,
    const char * imagefilename, const char * textfilename
) {
    std::ifstream file(textfilename);
    if (!file) {
        std::cerr << strerror(errno) << '\n';
        return 2;
    }

    Image img = image_from_file(imagefilename);
    //std::cerr << img << '\n';

    size_t x = 0;
    Bounds bounds(img.width(), img.height());

    std::string s;

    while (auto const cbox = make_box_character(img, {x, 0}, bounds)) {
        //std::cerr << "\nbox(" << cbox << ")\n";

        if (!(file >> s)) {
            std::cerr << "definition not found\n";
            return 10;
        }

        definitions.emplace_back(s, img.section(cbox.index(), cbox.bounds()), loader);
        assert(definitions.back().datas == definitions.back().datas);

        x = cbox.x() + cbox.w();
    }

    if (file >> s) {
        std::cerr << "box character not found\n";
        return 20;
    }

    return 0;
}

int main(int ac, char **av)
{
    if (ac < 3 || !(ac & 1)) {
        std::cerr << av[0] << "image_file text_file [image_file text_file ...]\n";
        return 1;
    }

    DataLoader loader;
    all_registy(loader);

    std::vector<Definition> definitions;

    int return_code = 0;

    for (int i = 1; i < ac; i += 2) {
        int const code = load_file(definitions, loader, av[i], av[i+1]);
        if (code) {
            return_code = code;
        }
    }

    unique_sort_definitions(definitions);

    for (auto & def : definitions) {
        write_definition(std::cout, def, loader);
    }

    return return_code;
}
