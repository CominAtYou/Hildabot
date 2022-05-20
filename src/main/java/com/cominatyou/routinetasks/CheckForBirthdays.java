package com.cominatyou.routinetasks;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collection;
import java.util.List;
import java.util.Optional;

import com.cominatyou.App;
import com.cominatyou.db.RedisInstance;
import com.cominatyou.util.Values;
import com.cominatyou.util.logging.Log;

import org.atteo.evo.inflector.English;
import org.javacord.api.DiscordApi;
import org.javacord.api.entity.channel.TextChannel;
import org.javacord.api.entity.permission.Role;
import org.javacord.api.entity.user.User;
import org.quartz.Job;
import org.quartz.JobExecutionContext;
import org.quartz.JobExecutionException;

public class CheckForBirthdays implements Job {
    @Override
    public void execute(JobExecutionContext context) throws JobExecutionException {
        final DiscordApi client = App.getClient();

        Log.event("BIRTHDAYS", "Starting birthdays task.");

        final Optional<Role> birthdayRole = client.getServerById(Values.HILDACORD_ID).get().getRoleById(609258045759029250L);
        birthdayRole.ifPresent(role -> {
            final Collection<User> birthdayRoleUsers = role.getUsers();
            if (birthdayRoleUsers.size() != 0) System.out.printf("[BIRTHDAYS] Removing birthday role from %d %s\n", birthdayRoleUsers.size(), English.plural("user", birthdayRoleUsers.size()));
            birthdayRoleUsers.forEach(user -> {
                user.removeRole(role, "Their birthday has passed.");
            });
        });

        final int month = Calendar.getInstance().get(Calendar.MONTH) + 1; // January is 0
        final int day = Calendar.getInstance().get(Calendar.DAY_OF_MONTH);

        final String monthString = month < 10 ? "0" + month : String.valueOf(month);
        final String dayString = day < 10 ? "0" + day : String.valueOf(day);

        final ArrayList<String> birthdays = new ArrayList<>(RedisInstance.getInstance().lrange(String.format("birthdays:%s:%s", monthString, dayString), 0, -1));

        if (Calendar.getInstance().get(Calendar.YEAR) % 4 != 0 && month == 3 && day == 1) {
            final List<String> leapBirthdays = RedisInstance.getInstance().lrange("birthdays:02:29", 0, -1);
            birthdays.addAll(leapBirthdays);
        }
        final TextChannel birthdayChannel = client.getServerById(Values.HILDACORD_ID).get().getTextChannelById(609253148564914187L).get();

        final int birthdaysCount = birthdays.size();

        Log.eventf("BIRTHDAYS", "Got %d %s for %d-%s\n", birthdaysCount, English.plural("birthday", birthdaysCount), month, dayString);

        // Filter out members that are no longer in the server
        birthdays.forEach(i -> {
            if (client.getServerById(Values.HILDACORD_ID).get().getMemberById(i).isEmpty()) {
                birthdays.remove(i);
            }
        });

        if (birthdays.size() == 0) {
            return;
        }
        else if (birthdays.size() == 1) {
            birthdayChannel.sendMessage(String.format("Hey <@%s>! I just wanted to wish you the happiest of birthdays! Can I have a slice of cake too? :birthday: :heart:", birthdays.get(0)));
        }
        else if (birthdays.size() == 2) {
            birthdayChannel.sendMessage(String.format("Hey <@%s> and <@%s>! I just wanted to wish you both the happiest of birthdays! Can I have a slice of cake too? :birthday: :heart:", birthdays.get(0), birthdays.get(1)));
        }
        else {
            StringBuilder announcement = new StringBuilder("Hey");
            for (int i = 0; i < birthdays.size(); i++) {
                if (i != birthdays.size() - 1) announcement.append(" <@" + birthdays.get(i) + ">, ");
                else announcement.append("and <@" + birthdays.get(i) + ">! ");
            }
            announcement.append("I just wanted to wish you all the happiest of birthdays! Can I have a slice of cake too? :birthday: :heart:");
            birthdayChannel.sendMessage(announcement.toString());
        }

        birthdays.forEach(id -> {
            client.getServerById(Values.HILDACORD_ID).get().getMemberById(id).ifPresent(user -> {
                birthdayRole.ifPresent(role -> {
                    role.addUser(user, "Their birthday is today!");
                    Log.eventf("BIRTHDAYS", "Gave birthday role to %s (%d)\n", user.getDiscriminatedName(), user.getId());
                });
            });
        });
    }
}
