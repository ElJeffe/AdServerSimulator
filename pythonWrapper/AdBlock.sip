// define the SIP wrapper for an AdBlock

%ModuleCode
#include "channelconfig.h"
%End

%Import QtCore/QtCoremod.sip


class CAdBlock: QObject
{
%TypeHeaderCode
#include "channelconfig.h"
%End

public:
  QString m_name;
  QDateTime m_startTime;
  QTime m_recurringTime;
  CAdBlock();
  void addAdConfig(const CAdConfig& adConfig);
};

