package com.cominatyou.store.items;

import java.time.ZonedDateTime;

import org.javacord.api.entity.message.embed.EmbedBuilder;
import org.javacord.api.interaction.ButtonInteraction;

import com.cominatyou.db.RedisUserEntry;
import com.cominatyou.store.StoreItem;
import com.cominatyou.store.SuccessfulPurchaseEmbed;
import com.cominatyou.util.Values;

public class LargeSubmitBoost extends StoreItem {
    public LargeSubmitBoost(String name, String description, String additionalInfo, String id, int cost, PurchaseLimit limit) throws IllegalArgumentException {
        super(name, description, additionalInfo, id, cost, limit);
    }

    public void giveItem(ButtonInteraction interaction) {
        final RedisUserEntry user = new RedisUserEntry(interaction.getUser());

        final long oneWeek = ZonedDateTime.now(Values.TIMEZONE_AMERICA_CHICAGO)
            .toLocalDate()
            .plusDays(1)
            .atStartOfDay(Values.TIMEZONE_AMERICA_CHICAGO)
            .plusWeeks(1)
            .toEpochSecond();

        user.set("items:" + getId() + ":purchasableagain", String.valueOf(oneWeek));
        user.expireKeyAt("items:" + getId() + ":purchasableagain", oneWeek);

        user.decrementKey("tokens", getPrice());
        user.pushToList("items:submitboosts", "1.25", "1.25", "1.25");

        final EmbedBuilder embed = SuccessfulPurchaseEmbed.create(interaction.getUser(), "Your next 3 submissions will be given a 25% increase in XP.");

        interaction.getMessage().createUpdater()
            .removeAllComponents()
            .removeAllEmbeds()
            .setContent("")
            .addEmbed(embed)
            .applyChanges();

        interaction.acknowledge();
    }
}
