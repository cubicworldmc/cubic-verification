#include "bot.hpp"

static std::string get_token(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("failed to open token file: " + path);

    std::string token;
    std::getline(file, token);

    return token;
}

Bot::Bot(const std::string& token_file, const std::string& config_file,
         Localization& local)
    : src(get_token(token_file)),
      config(std::make_unique<File>(config_file)),
      local(local) {
    src.on_log(dpp::utility::cout_logger());

    src.on_ready([this](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            register_commands();
            register_events();
        }
    });
}

void Bot::register_commands() {
    std::cout << config->get<dpp::snowflake>("guild-id") << std::endl;
    WelcomeCommand::make(src, local, config->get<dpp::snowflake>("guild-id"));
    std::cout << "commands registred" << std::endl;
    std::cout << src.me.id << std::endl;
}

void Bot::register_events() {
    src.on_button_click([this](const dpp::button_click_t& event) {
        if (event.custom_id == "apply-button") {
            event.dialog(
                Modal::trigger_client(src, local.find(event.command.member)));
        }
    });

    src.on_form_submit([this](const dpp::form_submit_t& event) {
        if (event.custom_id == "modal-client") {
            const auto& components = event.components;

            dpp::embed embed;
            embed.set_title("New whitelist application");
            embed.set_color(0x5865F2);
            embed.add_field("Discord",
                            event.command.usr.get_mention() + " (" +
                                event.command.usr.username + ")",
                            false);

            for (const auto& c : components) {
                embed.add_field(c.custom_id, std::get<std::string>(c.value),
                                false);
            }

            dpp::component accept;
            accept.set_type(dpp::cot_button)
                .set_style(dpp::cos_success)
                .set_label("Accept")
                .set_id("application-accept:" +
                        std::to_string(event.command.usr.id));

            dpp::component reject;
            reject.set_type(dpp::cot_button)
                .set_style(dpp::cos_danger)
                .set_label("Reject")
                .set_id("application-reject:" +
                        std::to_string(event.command.usr.id));

            dpp::message msg;
            msg.set_channel_id(config->get<dpp::snowflake>("staff-channel-id"));
            msg.add_embed(embed);
            msg.add_component(dpp::component()
                                  .set_type(dpp::cot_action_row)
                                  .add_component(accept)
                                  .add_component(reject));

            src.message_create(msg);
            event.reply(dpp::message("Your application has been sumbitted")
                            .set_flags(dpp::m_ephemeral));
        }
    });
}

void Bot::run() { src.start(dpp::st_wait); }
