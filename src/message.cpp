#include "message.h"

Message::Message(char *Topic, char *message)
{
  this->Topic = Topic;
  this->message = message;
}
