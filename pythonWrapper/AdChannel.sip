// define the SIP wrapper for an AdChannel

%ModuleCode
#include "adchannel.h"
%End

%Import QtCore/QtCoremod.sip


class CAdChannel : QObject
{
%TypeHeaderCode
#include "adchannel.h"
%End

public:
  CAdChannel(QObject *parent /TransferThis/ = 0);
  void setChannelConfig(const CChannelConfig& channelConfig);
  bool connectSplicer();
  bool disconnectSplicer();
  CAdState::AdState_t state() const;
signals:
  void stateChanged(CAdState::AdState_t newState);
};

