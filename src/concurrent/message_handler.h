/**
 * @file message_handler.h
 * @brief Parent for classes making use of simple messages.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#ifndef MESSAGE_HANDLER_H_
#define MESSAGE_HANDLER_H_

#include <mutex>
#include <vector>

#include "simple_message.h"

namespace rideshare {

class MessageHandler {
  public:
    // Message receiving
    virtual void Message(SimpleMessage simple_message) {};

  protected:
    // Message reading
    virtual void ReadMessages() {};

    // Store received messages
    std::vector<SimpleMessage> messages_;
    // Lock down messages_ while read/writing
    std::mutex messages_mutex_;
};

}  // namespace rideshare

#endif  // MESSAGE_HANDLER_H_