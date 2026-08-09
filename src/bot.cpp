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
      local(local),
      api(config->get<std::string>("api-host"), config->get<size_t>("api-port"),
          config->get<std::string>("ca-cert"),
          config->get<std::string>("client-cert"),
          config->get<std::string>("client-key")),
      crypto(config->get<std::string>("key-file"),
             config->get<bool>("key-is-hex")) {
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
            return;
        }

        if (event.custom_id.rfind("application-accept:", 0) == 0) {
            std::string data =
                event.custom_id.substr(strlen("application-accept:"));
            size_t pos = data.find(':');

            std::string user_id = data.substr(0, pos);
            std::string code = data.substr(pos + 1);

            api.accept("whitelist", code);

            src.direct_message_create(
                dpp::snowflake(user_id),
                dpp::message("Your application has been accepted"));

            src.message_delete(event.command.msg.id,
                               event.command.msg.channel_id);

            event.reply(dpp::message("Application accepted")
                            .set_flags(dpp::m_ephemeral));
            return;
        }

        if (event.custom_id.rfind("application-reject:", 0) == 0) {
            std::string data =
                event.custom_id.substr(strlen("application-reject:"));
            size_t pos = data.find(':');

            std::string user_id = data.substr(0, pos);
            std::string code = data.substr(pos + 1);

            api.decline("whitelist", code);

            event.dialog(Modal::trigger_reject(src, user_id));
            src.message_delete(event.command.msg.id,
                               event.command.msg.channel_id);
            return;
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

            std::string nickname;
            std::string code;
            for (const auto& c : components) {
                if (c.custom_id == "form-client-id1")
                    nickname = std::get<std::string>(c.value);
                if (c.custom_id == "form-client-code")
                    code = std::get<std::string>(c.value);

                embed.add_field(c.custom_id, std::get<std::string>(c.value),
                                false);
            }

            std::string decrypted = to_lower(crypto.decrypt(code));
            if (to_lower(nickname) != decrypted) {
                event.reply(
                    dpp::message(config->get<std::string>("code-invalid"))
                        .set_flags(dpp::m_ephemeral));
                return;
            }

            dpp::component accept;
            accept.set_type(dpp::cot_button)
                .set_style(dpp::cos_success)
                .set_label("Accept")
                .set_id("application-accept:" +
                        std::to_string(event.command.usr.id) + ":" + code);

            dpp::component reject;
            reject.set_type(dpp::cot_button)
                .set_style(dpp::cos_danger)
                .set_label("Reject")
                .set_id("application-reject:" +
                        std::to_string(event.command.usr.id) + ":" + code);

            dpp::message msg;
            msg.set_channel_id(config->get<dpp::snowflake>("staff-channel-id"));
            msg.add_embed(embed);
            msg.add_component(dpp::component()
                                  .set_type(dpp::cot_action_row)
                                  .add_component(accept)
                                  .add_component(reject));

            src.message_create(msg);
            event.reply(
                dpp::message(local.find(event.command.member)
                                 .get<std::string>("application-sumbit"))
                    .set_flags(dpp::m_ephemeral));
            return;
        }

        if (event.custom_id.rfind("modal-reject:", 0) == 0) {
            dpp::snowflake user_id(
                event.custom_id.substr(strlen("modal-reject:")));
            std::string reason =
                std::get<std::string>(event.components[0].value);

            dpp::embed embed;
            embed.set_color(REG_CR2);
            embed.set_title(config->get<std::string>("form-reject-title"));
            embed.add_field(config->get<std::string>("form-reject-reason"),
                            reason, false);

            dpp::message msg;
            msg.add_embed(embed);
            src.direct_message_create(user_id, msg);
            event.reply(
                dpp::message("user notified").set_flags(dpp::m_ephemeral));
            return;
        }
    });
}

void Bot::run() { src.start(dpp::st_wait); }

std::string Bot::to_lower(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    return str;
}
