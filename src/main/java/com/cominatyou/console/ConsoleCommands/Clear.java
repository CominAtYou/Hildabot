package com.cominatyou.console.ConsoleCommands;

import java.util.List;

public class Clear implements ConsoleCommand {
    public void execute(List<String> args) {
        System.out.print("\033[H\033[2J");
    }
}
