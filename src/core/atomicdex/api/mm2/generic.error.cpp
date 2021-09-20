//
// Created by Sztergbaum Roman on 08/06/2021.
//

#include "atomicdex/api/mm2/generic.error.hpp"

namespace mm2::api
{
    void
    from_json(const nlohmann::json& j, generic_answer_error& res)
    {
        j.at("error").get_to(res.error);
        j.at("error_path").get_to(res.error_path);
        j.at("error_trace").get_to(res.error_trace);
        j.at("error_type").get_to(res.error_type);
        j.at("error_data").get_to(res.error_data);
    }
} // namespace mm2::api