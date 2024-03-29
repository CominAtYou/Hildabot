package com.cominatyou.commands.admin;

import java.util.List;

import com.cominatyou.TextCommand;
import com.cominatyou.util.versioning.Version;

import org.javacord.api.event.message.MessageCreateEvent;

public class GetVersion extends TextCommand {
    public void execute(MessageCreateEvent message, List<String> messageArgs) {
        if (!message.getMessageAuthor().isBotOwner()) return;
        message.getChannel().sendMessage("Version " + Version.VERSION_STRING);
    }
}
