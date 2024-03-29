package com.cominatyou.commands;

import java.util.List;

import org.javacord.api.event.message.MessageCreateEvent;

import com.cominatyou.TextCommand;
import com.cominatyou.commands.birthdays.*;
import com.cominatyou.util.MessageUtil;

public class Birthdays extends TextCommand {
    public void execute(MessageCreateEvent message, List<String> messageArgs) {
        if (messageArgs.isEmpty() || messageArgs.size() == 1 && !messageArgs.get(0).equalsIgnoreCase("list")) {
            MessageUtil.sendTextReply(message.getMessage(), "Looks like you're missing some arguments. Please make sure you provided a command (set|edit) and a date (i.e. `h!birthday set 06-21`), or list and a month if you want to view upcoming birthdays!");
        }
        else if (messageArgs.get(0).equalsIgnoreCase("set")) {
            SetBirthday.set(message, messageArgs);
        } else if (messageArgs.get(0).equalsIgnoreCase("edit")) {
            EditBirthday.edit(message, messageArgs);
        }
        else if (messageArgs.get(0).equalsIgnoreCase("list")) {
            ListBirthdays.list(message, messageArgs);
        }
    }
}
