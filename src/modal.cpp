#include "modal.hpp"

dpp::interaction_modal_response Modal::trigger_client(const dpp::cluster& src,
                                                      File& local) {
    dpp::interaction_modal_response res(
        "modal-client", local.get<std::string>("form-client-title"));

    res.add_component(
        dpp::component()
            .set_label(local.get<std::string>("form-client-name"))
            .set_id("form-client-id1")
            .set_type(dpp::cot_text)
            .set_placeholder(local.get<std::string>("form-client-name-ph"))
            .set_text_style(dpp::text_short));

    res.add_component(
        dpp::component()
            .set_label(local.get<std::string>("form-client-code"))
            .set_id("form-client-code")
            .set_type(dpp::cot_text)
            .set_placeholder(local.get<std::string>("form-client-code-ph"))
            .set_text_style(dpp::text_short));

    size_t qsize = local.get<size_t>("form-client-q-size");
    for (size_t i = 0; i < qsize; i++) {
        std::string raw = "form-client-q" + std::to_string(i + 2);
        res.add_component(
            dpp::component()
                .set_label(local.get<std::string>(raw + "-name"))
                .set_id(raw)
                .set_type(dpp::cot_text)
                .set_placeholder(local.get<std::string>(raw + "-ph"))
                .set_text_style(dpp::text_short));
    }

    return res;
}

dpp::interaction_modal_response Modal::trigger_reject(
    const dpp::cluster& src, const std::string user_id) {
    dpp::interaction_modal_response res("modal-reject:" + user_id,
                                        "Reject application");
    res.add_component(dpp::component()
                          .set_label("Reason")
                          .set_id("reason")
                          .set_type(dpp::cot_text)
                          .set_text_style(dpp::text_paragraph));

    return res;
}
