// define the SIP wrapper for an AdConfig

%ModuleCode
#include "channelconfig.h"
%End

%Import QtCore/QtCoremod.sip


class CAdConfig
{
%TypeHeaderCode
#include "channelconfig.h"
%End

public:
  quint16 m_serviceId;
  quint16 m_pcrPid;
  quint32 m_duration;
  quint32 m_postBlack;
  quint8 m_accessType;
  bool m_overridePlaying;
  bool m_returnToPriorChannel;
  CAdConfig();
  CAdConfig(quint16 pcrPid, quint16 serviceId = 0xFFFF, quint32 durationInTicks = 0xFFFFFFFF,
            quint32 postBlackInTicks = 0xFFFFFFFF, quint8 accessType = 0, bool overridePlaying = false,
            bool returnToPriorChannel = true);
  void addElementaryStream(const CSpliceElementaryStream& spliceElementaryStream);
};

