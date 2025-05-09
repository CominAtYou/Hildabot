#include "activity_swapper.h"
#include <dpp/dpp.h>
#include <random>
#include "scheduler/instance.h"

static bool executed = false;

static const dpp::presence activities[] = {
    dpp::presence(dpp::ps_online, dpp::at_watching, "for your submissions!"),
    dpp::presence(dpp::ps_online, dpp::at_watching, "Frida practice magic"),
    dpp::presence(dpp::ps_online, dpp::at_watching, "Hilda and the Mountain King"),
    dpp::presence(dpp::ps_online, dpp::at_watching, "the sunset over Trolberg"),
    dpp::presence(dpp::ps_online, dpp::at_game, "with the Sparrow Scouts"),
    dpp::presence(dpp::ps_online, dpp::at_game, "with Twig"),
    dpp::presence(dpp::ps_online, dpp::at_game, "with the water spirits"),
    dpp::presence(dpp::ps_online, dpp::at_competing, "a dodgeball game"),
    dpp::presence(dpp::ps_online, dpp::at_listening, "the Rat King's secrets"),
    dpp::presence(dpp::ps_online, dpp::at_listening, "Alfur lecture"),
    dpp::presence(dpp::ps_online, dpp::at_listening, "the bells"),
    dpp::presence(dpp::ps_online, dpp::at_listening, "a campfire story"),
    dpp::presence(dpp::ps_online, dpp::at_listening, "The Life of Hilda"),
};

namespace activity_swapper {
    void swap_activity(dpp::cluster* bot) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(0, 13 - 1);

        const int random_index = dis(gen);
        const dpp::presence activity = activities[random_index];

        bot->set_presence(activity);
    }

    void start(dpp::cluster* bot) {
        if (!executed) {
            executed = true;

            swap_activity(bot);
            scheduler.every(std::chrono::minutes(10), swap_activity, bot);
        }
    }
}
