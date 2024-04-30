#include "UI/Menu.h"

#include <format>
#include <iostream>

#include "UI/Input.h"
#include "UI/Term.h"

namespace maa::cli::ui
{

coro::Promise<size_t> menu(std::string title, std::vector<MenuEntry> entries)
{
    cursor_save_helper __save;
    cursor_hide_helper __hide;
    erase::after();
    std::cout << title << std::endl << std::endl;

    for (size_t i = 0; i < entries.size(); i++) {
        const auto& entry = entries[i];
        std::cout << "  " << (i + 1) << ". " << entry.text << std::endl;
    }

    cursor::up(entries.size() + 1);

    size_t current = 0;
    do {
        cursor_save_helper __save2;
        cursor::down_left(current + 1);
        std::cout << '*' << std::flush;

        size_t new_current = current;
        switch (co_await input_raw()) {
        case 'w':
            new_current = current == 0 ? current : current - 1;
            break;
        case 's':
            new_current = current == entries.size() - 1 ? current : current + 1;
            break;
        case '\n':
            __save2.release();
            __hide.release();
            __save.release();
            if (entries[current].action) {
                co_await entries[current].action(current);
            }
            co_return current;
        }
        if (current != new_current) {
            cursor::to_col(0);
            std::cout << ' ' << std::flush;
            current = new_current;
        }
    } while (true);
}

}
