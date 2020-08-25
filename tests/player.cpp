//
// Created by Riley Quinn on 8/17/20.
//

#include <libaudio/player.hpp>
#include <libaudio/detail/guard.hpp>
#include <iostream>
#include <thread>
#include <options/options.hpp>

#if USE_CURSES

#include <ncurses.h>

void draw_progress_box(int x, int y, int w, char ch, float progress) {
    mvaddch(y, x, '[');
    mvaddch(y, x + w, ']');
    auto spaces = int(float(w) * progress);
    while (spaces--) {
        mvaddch(y, ++x, ch);
    }
}

#endif

inline int minutes(double seconds) {
    return static_cast<int>(seconds / 60);
}

inline int seconds(double seconds) {
    return static_cast<int>(seconds) % 60;
}

std::string usage(options::option_spec &spec, std::string_view prefix) {
    std::string result = std::string{prefix};

    std::vector<std::string> options;

    for (const auto &[_, option] : spec.get_options()) {
        std::string str;
        if (!option.required()) {
            str += "[" + option.name();
            if (option.has_value()) {
                str += " value";
            }
            str += "]";
        } else {
            str += option.name();
            if (option.has_value()) {
                str += " value";
            }
        }
        options.push_back(str);
    }

    for (const auto &option : options) {
        result += " " + option;
    }

    return result;
}

int main(int argc, const char *argv[]) {
    using options::option;

    options::app app;
    app.name("Music Player")
            .description("A music player using libaudio")
            .add_option(option{"--help", 'h', "Show this help menu"})
            .add_option(option{"--seek-time", 's', "How many seconds to seek"}
                                .default_value("10.0")
                                .validator([](const options::option &option) -> bool {
                                    for (bool seen_dot = false; char c : option.value()) {
                                        if (!std::isdigit(c)) {
                                            if (c == '.' && !seen_dot) {
                                                seen_dot = true;
                                            } else {
                                                return false;
                                            }
                                        }
                                    }
                                    return true;
                                }));
    #if USE_CURSES
    app.add_option(option{"--disable-ui", 'd', "Disable the ncurses UI"});
    #endif

    options::parse_result result = options::parse_options(app, argc, argv);

    if (result.remaining.empty() || result["--help"]) {
        app.usage(usage(app.option_spec(), std::string{argv[0]} + " <file>"));
        std::cout << app.help() << std::endl;
        return 0;
    }

    std::string_view file = result.remaining[0];

    if (!std::filesystem::exists(file)) {
        std::cerr << file << ": file does not exist\n";
        return 1;
    } else if (!std::filesystem::is_regular_file(file)) {
        std::cerr << file << ": not a file\n";
        return 1;
    }

    Player player;
    try {
        player.load_file(file);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    #if USE_CURSES
    if (result["--disable-ui"]) {
    #endif

    std::cout << std::setfill('0') << "Playing " << file << " for " << std::setw(2) << minutes(player.duration())
              << ':' << std::setw(2) << seconds(player.duration()) << std::endl;
    player.play();
    while (player.playing()) player.wait_for_completion(0.1);

    return 0;

    #if USE_CURSES
    }

    double seek_time = std::stod(result["--seek-time"].value());

    initscr();
    raw();
    keypad(stdscr, true);
    noecho();
    scrollok(stdscr, true);
    curs_set(false);
    nodelay(stdscr, true);

    DEFER(endwin);

    player.play();
    player.set_callback([](Player &player) {
        player.stop();
    });

    int c, x, y;
    bool running = true;
    while (running && (c = getch())) {
        switch (c) {
            case 'p':
            case ' ':player.playing() ? player.pause() : player.play();
                break;
            case 'r':player.set_playhead(0.0);
                player.play();
                break;
            case KEY_LEFT:player.seek(-seek_time);
                break;
            case KEY_RIGHT:player.seek(seek_time);
                break;
            case KEY_RESIZE:break;
            case static_cast<unsigned char>('c') & 0x1fu:
            case 'q':
            case 27:running = false;
                break;
            default:break;
        }

        if (!running) break;

        clear();
        auto progress = player.current_time() / player.duration();
        getmaxyx(stdscr, y, x);
        mvprintw(y / 2 - 1, x / 2 - int(file.length() + 9) / 2, "Playing: %s", file.data());
        draw_progress_box(x / 4, y / 2, x / 2, '#', progress);
        char buf[64];
        sprintf(buf, "%02d:%02d/%02d:%02d",
                minutes(player.current_time()),
                seconds(player.current_time()),
                minutes(player.duration()),
                seconds(player.duration()));
        std::string_view str{buf};
        mvprintw(y / 2 + 1, x / 2 - int(str.size() / 2 + 1), "%s", str);
        sprintf(buf, "p/space:toggle   r:restart   q/esc:quit   left/right:seek %.1fs", seek_time);
        str = buf;
        mvprintw(y - 1, x / 2 - int(str.size() / 2 + 1), "%s", str);
        refresh();
        player.wait_for_completion(0.1);
    }

    return 0;
    #endif
}