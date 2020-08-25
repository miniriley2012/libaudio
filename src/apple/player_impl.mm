//
// Created by Riley Quinn on 8/19/20.
//

#include "libaudio/player.hpp"
#include <thread>
#import <AVFoundation/AVAudioPlayer.h>
#import "objc/runtime.h"

@interface AudioPlayer : AVAudioPlayer <AVAudioPlayerDelegate>
@end

@implementation AudioPlayer
- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag {
    auto cpp_player = (__bridge Player *) objc_getAssociatedObject(self, "player");
    if (cpp_player && cpp_player->callback) {
        (cpp_player->callback)(*cpp_player);
    }
}

- (void)audioPlayerDecodeErrorDidOccur:(AVAudioPlayer *)player error:(NSError *)error {
    // TODO handle error
}
@end

class Player::impl {
public:
    explicit impl(Player *parent) : parent(parent) {}

    void load_file(const std::filesystem::path &path) {
        const auto path_str = std::filesystem::absolute(path).string();
        auto url = [NSURL fileURLWithFileSystemRepresentation:path_str.data() isDirectory:false relativeToURL:nullptr];
        NSError *__autoreleasing error;
        player = [[AudioPlayer alloc] initWithContentsOfURL:(url) error:&error];
        if (player == nullptr) {
            // TODO replace with custom exception
            throw std::runtime_error{"Could not initialize player. The file is most likely uses an unsupported format."};
        }

        objc_setAssociatedObject(player, "player", (__bridge id) parent, OBJC_ASSOCIATION_ASSIGN);

        player.delegate = player;

        if (![player prepareToPlay]) {
            // TODO handle player prepareToPlay failure
            exit(1);
        }
    }

    void wait_for_completion(double timeout = -1) const {
        CFRunLoopRunInMode(kCFRunLoopDefaultMode,
                           timeout > 0 ? timeout : std::numeric_limits<CFTimeInterval>::max(), false);
    }

    void play() {
        [player play];
    }

    void pause() {
        [player pause];
    }

    void stop() {
        [player stop];
    }

    void set_playhead(double seconds) {
        player.currentTime = std::clamp(seconds, 0.0, duration() - 0.01);
    }

    void seek(double seconds) {
        set_playhead(current_time() + seconds);
    }

    [[nodiscard]] double duration() const {
        return player.duration;
    }

    void set_volume(double volume) {
        player.volume = static_cast<float>(volume);
    }

    [[nodiscard]] double volume() const {
        return player.volume;
    }

    [[nodiscard]] bool playing() const {
        return player.playing;
    };

    [[nodiscard]] double current_time() const {
        return player.currentTime;
    }

private:
    Player *parent;
    AudioPlayer *player;
};

#include "../player.cpp"