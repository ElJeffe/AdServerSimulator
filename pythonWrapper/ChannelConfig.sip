// define the SIP wrapper for an ChannelConfig

%ModuleCode
#include "channelconfig.h"
%End

%Import QtCore/QtCoremod.sip


class CChannelConfig
{
%TypeHeaderCode
#include "channelconfig.h"
%End

public:
  QString m_splicerAddress;
  QString m_channelName;
  QString m_splicerName;
  int m_chassis;
  int m_card;
  int m_port;
  int m_secondsBeforeSpliceRequest;
  bool m_processScte35;
  CChannelConfig();
  void addAdBlock(const CAdBlock& adBlock);
};

