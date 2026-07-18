#include "command.hpp"

void WelcomeCommand::make(dpp::cluster& src, const std::unique_ptr<File>& local,
                          const dpp::snowflake& guild_id) {
    src.guild_command_create(
        dpp::slashcommand("cubic-verification-welcome",
                          "cubic verification welcome", src.me.id),
        guild_id);

    src.on_slashcommand([local =
                             local.get()](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "cubic-verification-welcome") {
            dpp::embed embed;
            embed.set_title(local->get<std::string>("welcome-title"));
            embed.set_description(local->get<std::string>("welcome-desc"));
            embed.set_color(0x57F287);
            embed.set_image(local->get<std::string>("welcome-image"));

            event.reply(dpp::message().add_embed(embed));
        }
    });
}
