#include "command.hpp"

void WelcomeCommand::make(dpp::cluster& src, Localization& local,
                          const dpp::snowflake& guild_id) {
    dpp::slashcommand cmd("cubic-verification-welcome",
                          "cubic verification welcome", src.me.id);
    cmd.add_option(dpp::command_option(dpp::co_sub_command, "en", "en"));
    cmd.add_option(dpp::command_option(dpp::co_sub_command, "ru", "ru"));
    cmd.add_option(dpp::command_option(dpp::co_sub_command, "es", "es"));

    src.guild_command_create(cmd, guild_id);

    src.on_slashcommand([&local](const dpp::slashcommand_t& event) {
        dpp::command_interaction data = event.command.get_command_interaction();
        if (event.command.get_command_name() == "cubic-verification-welcome") {
            if (data.options.size() != 1) {
                event.reply("usage: cmd <en:ru:es>");
                return;
            }
            auto sub = data.options[0];

            File& lang = local.get(sub.name);

            dpp::embed embed;
            embed.set_title(lang.get<std::string>("welcome-title"));
            embed.set_description(lang.get<std::string>("welcome-desc"));
            embed.set_color(0x57F287);
            embed.set_image(lang.get<std::string>("welcome-image"));

            dpp::component button;
            button.set_type(dpp::cot_button)
                .set_style(dpp::cos_success)
                .set_label(lang.get<std::string>("welcome-button"))
                .set_id("apply-button");

            dpp::message msg;
            msg.add_embed(embed);
            msg.add_component(dpp::component()
                                  .set_type(dpp::cot_action_row)
                                  .add_component(button));

            event.reply(msg);
        }
    });
}
