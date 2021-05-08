/**
 * @file simple_message.h
 * @brief Store two ints, one a message code, and the other an id (e.g. passenger id).
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#ifndef SIMPLE_MESSAGE_H_
#define SIMPLE_MESSAGE_H_

namespace rideshare {

struct SimpleMessage {
    int message_code; // should come from enum of class receiving message
    int id; // passenger or vehicle id
};

}  // namespace rideshare

#endif  // SIMPLE_MESSAGE_H_