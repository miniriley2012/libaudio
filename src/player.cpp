//
// Created by Riley Quinn on 8/18/20.
//

#include "libaudio/player.hpp"

Player::Player() {
    internal = std::make_unique<impl>(this);
}

Player::Player(const std::filesystem::path &path) : Player() {
    load_file(path);
}

Player::~Player() = default;

void Player::load_file(const std::filesystem::path &path) {
    internal->load_file(std::filesystem::absolute(path).string());
}

void Player::wait_for_completion(double timeout) {
    internal->wait_for_completion(timeout);
}

void Player::play() {
    internal->play();
}

void Player::pause() {
    internal->pause();
}

void Player::stop() {
    internal->stop();
}

void Player::set_playhead(double seconds) {
    internal->set_playhead(seconds);
}

void Player::seek(double seconds) {
    internal->seek(seconds);
}

float Player::duration() {
    return internal->duration();
}

void Player::set_volume(double volume) {
    return internal->set_volume(volume);
}

double Player::volume() {
    return internal->volume();
}

bool Player::playing() {
    return internal->playing();
}

double Player::current_time() {
    return internal->current_time();
}

void Player::set_callback(const callback_type &callback) {
    this->callback = callback;
}
