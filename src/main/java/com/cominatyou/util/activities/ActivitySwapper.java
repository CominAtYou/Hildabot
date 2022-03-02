package com.cominatyou.util.activities;

import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.ThreadLocalRandom;

import com.cominatyou.util.versioning.Version;

import org.javacord.api.DiscordApi;
import org.javacord.api.entity.activity.ActivityType;

public class ActivitySwapper {
    private static final Activity[] activities = { new Activity(ActivityType.PLAYING, "with Twig"),
            new Activity(ActivityType.WATCHING, "the Woffs migrate"),
            new Activity(ActivityType.WATCHING, "for your submissions!"),
            new Activity(ActivityType.WATCHING, "Frida practice magic"),
            new Activity(ActivityType.WATCHING, "Hilda and the Mountain King"),
            new Activity(ActivityType.PLAYING, "with the Sparrow Scouts"),
            new Activity(ActivityType.PLAYING, "with the trolls"),
            new Activity(ActivityType.PLAYING, String.format("Version %s / Build %s", Version.VERSION, Version.BUILD_NUMBER)),
            new Activity(ActivityType.COMPETING, "a dodgeball game")
    };

    public static void start(DiscordApi client) {
        new Timer().scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                final int index = ThreadLocalRandom.current().nextInt(0, activities.length);
                client.updateActivity(activities[index].getType(), activities[index].getActivity());
            }
        }, 0, 600000); // Swaps activities every ten minutes.
    }
}
