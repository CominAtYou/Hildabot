package com.cominatyou.commands;

import java.util.List;
import java.util.Map;
import static java.util.Map.entry;

import com.cominatyou.TextCommand;
import com.cominatyou.helparticles.*;
import com.cominatyou.util.MessageUtil;

import org.javacord.api.entity.message.embed.EmbedBuilder;
import org.javacord.api.event.message.MessageCreateEvent;

public class Help extends TextCommand {
    final static Map<String, EmbedBuilder> helpEmbeds = Map.ofEntries(
        entry("birthday", BirthdayHelp.ARTICLE_EMBED),
        entry("levelalert", LevelAlertHelp.ARTICLE_EMBED),
        entry("levelcheck", LevelCheckHelp.ARTICLE_EMBED),
        entry("stats", StatsHelp.ARTICLE_EMBED),
        entry("store", StoreHelp.ARTICLE_EMBED),
        entry("streakwarning", StreakWarningHelp.ARTICLE_EMBED),
        entry("submit", SubmitHelp.ARTICLE_EMBED),
        entry("timeleft", TimeLeftHelp.ARTICLE_EMBED)
    );

    public void execute(MessageCreateEvent message, List<String> args) {
        if (args.isEmpty()) {
            message.getChannel().sendMessage(AvailableHelpArticles.HELP_ARTICLES_EMBED);
            return;
        }

        if (!helpEmbeds.containsKey(args.get(0).toLowerCase())) {
            MessageUtil.sendTextReply(message.getMessage(), "There isn't a help article with that name!");
            return;
        }

        message.getChannel().sendMessage(helpEmbeds.get(args.get(0).toLowerCase()));
    }
}
