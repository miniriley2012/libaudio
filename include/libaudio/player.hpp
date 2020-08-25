//
// Created by Riley Quinn on 8/18/20.
//

#ifndef LIBAUDIO_LIBAUDIO_PLAYER_HPP
#define LIBAUDIO_LIBAUDIO_PLAYER_HPP


#include <filesystem>
#include <experimental/propagate_const>

class Player {
public:
    using callback_type = std::function<void(Player &)>;
    callback_type callback;

    Player();
    explicit Player(const std::filesystem::path &path);

    ~Player();

    void load_file(const std::filesystem::path &path);

    void wait_for_completion(double timeout = -1);

    void play();

    void pause();

    void stop();

    void set_playhead(double seconds);

    void seek(double seconds);

    float duration();

    void set_volume(double volume);

    double volume();

    bool playing();

    double current_time();

    void set_callback(const callback_type &callback);

private:
    class impl;
    std::experimental::propagate_const<std::unique_ptr<impl>> internal;
};


#endif //LIBAUDIO_LIBAUDIO_PLAYER_HPP
