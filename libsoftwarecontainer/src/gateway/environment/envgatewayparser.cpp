/*
 * Copyright (C) 2016-2017 Pelagicore AB
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
 * BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 * For further information see LICENSE
 */

#include "envgatewayparser.h"

namespace softwarecontainer {

bool EnvironmentGatewayParser::parseEnvironmentGatewayConfigElement(
    const json_t *element,
    EnvironmentVariable &result,
    const EnvironmentVariables &store)
{

    if (!requireNonEmptyKeyValue(element, "name", result.first)
        || !requireNonEmptyKeyValue(element, "value", result.second)) {
        return false;
    }

    std::string mode = "set";
    std::string separator = "";

    if (!JSONParser::readOptional(element, "mode", mode)) {
        log_error() << "Could not parse \"mode\" key";
        return false;
    }

    // Convert to lowercase
    std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

    std::vector<std::string> validModes = { "set", "prepend", "append" };
    if (validModes.end() == std::find(validModes.begin(), validModes.end(), mode)) {
        log_error() << "Invalid mode, only " << validModes << " are valid";
        return false;
    }

    if (!JSONParser::readOptional(element, "separator", separator)) {
        log_error() << "Could not parse \"separator\" key";
        return false;
    }

    if (store.count(result.first) == 0) {
        if (mode != "set") {
            log_info() << "Env variable \"" << result.first << "\" was configured to "
                       << "be appended/prepended but the variable has not previously"
                       << " been set, so it will be created. Value is set to: \""
                       << result.second << "\"";
        }
        return true;
    } else {
        if (mode == "append") {
            result.second = store.at(result.first) + separator + result.second;
            return true;
        } else if (mode == "prepend") {
            result.second = result.second + separator + store.at(result.first);
            return true;
        } else { // mode == set
            log_error() << "Env variable " << result.first
                        << " already defined with value : " << store.at(result.first);
            return false;
        }
    }
}

bool EnvironmentGatewayParser::requireNonEmptyKeyValue(const json_t *element,
                                                       const std::string key,
                                                       std::string &result)
{
    if (!JSONParser::read(element, key.c_str(), result)) {
        log_error() << "Key " << key << " missing or not a string in json configuration";
        return false;
    }

    if (result.length() == 0) {
        log_error() << "Value for " << key << " key is an empty string";
        return false;
    }

    return true;
}

} // namespace softwarecontainer
