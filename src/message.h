#ifndef Message_H
#define Message_H

class Message
{

public:
    Message(char *Topic, char *message);
    ~Message();
    char *message;
    char *Topic;

private:
};

#endif