package com.cominatyou.commands.admin;

import java.util.List;

import com.cominatyou.commands.interfaces.Command;
import com.cominatyou.db.RedisInstance;

import org.javacord.api.event.message.MessageCreateEvent;

public class Commit implements Command {
    public void execute(MessageCreateEvent message, List<String> messageArgs) {
        if (!message.getMessageAuthor().isBotOwner()) return;

        final String response = RedisInstance.getInstance().save();

        if (response.equalsIgnoreCase("ok")) {
            message.getChannel().sendMessage("Database has been written to disk!");
        }
        else {
            message.getChannel().sendMessage("Something went wrong. Info is available in the log.");
            System.err.println("[REDIS] Save failed: " + response);
        }
    }

    public String getName() {
        return "Commit";
    }
}
