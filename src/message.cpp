#include "message.h"

Message::Message(char *Topic, String messageData)
{
  this->Topic = Topic;
  this->msg = messageData;
}
