#include "activity_swapper.h"
#include <dpp/dpp.h>
#include <random>
#include <vector>
#include "scheduler/scheduler.h"

static bool executed = false;

static std::vector<dpp::presence> activities = {
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

static Bosma::Scheduler scheduler(12U);

namespace activity_swapper {
    void start(dpp::cluster* bot) {
        if (!executed) {
            executed = true;

            scheduler.every(std::chrono::minutes(10), [bot](std::vector<dpp::presence>* activities) {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                static std::uniform_int_distribution<> dis(0, activities->size() - 1);

                const int random_index = dis(gen);
                const dpp::presence activity = activities->at(random_index);

                bot->set_presence(activity);
            }, &activities);
        }
    }
}
