package com.cominatyou.commands.birthdays;

import java.text.DateFormatSymbols;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Calendar;
import java.util.List;
import java.util.Optional;

import org.javacord.api.entity.message.embed.EmbedBuilder;
import org.javacord.api.entity.user.User;
import org.javacord.api.event.message.MessageCreateEvent;

import com.cominatyou.db.RedisInstance;
import com.cominatyou.util.BirthdayEntry;
import com.cominatyou.util.Values;

public class ListBirthdays {
    private static final List<String> thirtyDayMonths = Arrays.asList("09", "04", "06", "11");

    public static void list(MessageCreateEvent message, List<String> messageArgs) {
        final Integer month;
        // If no month is provided, get birthdays for this month
        if (messageArgs.size() == 1) {
            month = Calendar.getInstance().get(Calendar.MONTH) + 1;
        }
        else try {
            month = Integer.valueOf(messageArgs.get(1));
        }
        catch (Exception e) {
            message.getMessage().reply(messageArgs.get(1) + " doesn't seem to be a month! Please specify a month from 1 - 12.");
            return;
        }

        // Single-digit numbers need a leading 0 for the DB.
        final String monthStr = month < 10 ? "0" + month : month.toString();
        final int numberOfDays = month == 2 ? 29 : (thirtyDayMonths.contains(monthStr) ? 30 : 31);
        final ArrayList<BirthdayEntry> birthdays = new ArrayList<>();

        for (int i = 1; i <= numberOfDays; i++) {
            final String day = i < 10 ? "0" + i : String.valueOf(i);
            final int dayInt = i; // needed for forEach loop below
            final List<String> birthdaysForDay = RedisInstance.getInstance().lrange(String.format("birthdays:%s:%s", monthStr, day), 0, -1);

            birthdaysForDay.forEach(id -> {
                birthdays.add(new BirthdayEntry(id, dayInt));
            });
        }

        final String monthString = DateFormatSymbols.getInstance().getMonths()[month - 1];
        final String shortenedMonthString = monthString.substring(0, 3);

        final EmbedBuilder embed = new EmbedBuilder()
            .setTitle(":birthday: Birthdays for " + monthString)
            .setColor(Values.HILDA_BLUE)
            .setDescription(birthdays.size() == 0 ? "No birthdays for this month!" : "");

        for (int i = 0; i < birthdays.size(); i++) {
            final BirthdayEntry entry = birthdays.get(i);
            final Optional<User> user = message.getApi().getServerById(Values.HILDACORD_ID).get().getMemberById(entry.getUserId());
            if (user.isEmpty()) continue; // User is not in guild

            if (i == 18) {
                if (birthdays.size() > 19) embed.addInlineField(birthdays.size() - 18 + " more", "up to " + month + "/" + numberOfDays);
                break;
            }
            else {
                embed.addInlineField(user.get().getDiscriminatedName(), shortenedMonthString + " " + entry.getDay());
            }
        }

        message.getChannel().sendMessage(embed);
    }
}
