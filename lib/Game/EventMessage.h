#ifndef EVENTMESSAGE_H
#define EVENTMESSAGE_H
#include "Domain/ModelBase.h"
class EventMessage {
  public:
    EventMessage(ModelBase &modelBase);
    ~EventMessage();
    ModelBase &getModelBase();
  protected:
  private:
    ModelBase &_modelBase;
};

#endif // EVENTMESSAGE_H
