package com.cominatyou.store.items;

import org.javacord.api.entity.message.embed.EmbedBuilder;
import org.javacord.api.interaction.ButtonInteraction;

import com.cominatyou.db.RedisUserEntry;
import com.cominatyou.store.StoreItem;
import com.cominatyou.store.SuccessfulPurchaseEmbed;
import com.cominatyou.xp.XPSystem;

public class SmallXPIncrease extends StoreItem {
    public SmallXPIncrease(String name, String description, String additionalInfo, String id, int price, PurchaseLimit purchaseLimit) throws IllegalArgumentException {
        super(name, description, additionalInfo, id, price, purchaseLimit);
    }

    public void giveItem(ButtonInteraction interaction) {
        final RedisUserEntry user = new RedisUserEntry(interaction.getUser());

        final int currentXP = user.getXP();
        user.decrementKey("tokens", getPrice());
        user.incrementKey("xp", 50);

        final EmbedBuilder embed = SuccessfulPurchaseEmbed.create(interaction.getUser(), "Your XP has increased by 50.");

        interaction.getMessage().createUpdater()
            .removeAllComponents()
            .removeAllEmbeds()
            .setContent("")
            .addEmbed(embed)
            .applyChanges();

        interaction.acknowledge();

        XPSystem.checkForLevelUp(currentXP, interaction.getUser(), interaction.getServer().get());
    }
}