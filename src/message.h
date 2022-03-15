#ifndef Message_H
#define Message_H
#include <Arduino.h>
class Message
{

public:
    Message(char *Topic, String messageData);
    ~Message();
    String msg;
    char *Topic;

private:
};

#endif