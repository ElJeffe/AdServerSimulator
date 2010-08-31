// define the SIP wrapper for a SpliceElementaryStream

%ModuleCode
#include "scte30messages.h"
%End

%Import QtCore/QtCoremod.sip


class CSpliceElementaryStream
{
%TypeHeaderCode
#include "scte30messages.h"
%End

public:
  quint16 m_pid;
  quint16 m_streamType;
  quint32 m_avgBitrate;
  quint32 m_maxBitrate;
  quint32 m_minBitrate;
  quint16 m_hResolution;
  quint16 m_vResolution;
  CSpliceElementaryStream();
  CSpliceElementaryStream(quint16 pid, quint16 streamType, quint32 avgBitrate = 0xFFFFFFFF,
               quint32 maxBitrate = 0xFFFFFFFF, quint32 minBitrate = 0xFFFFFFFF,
               quint16 hResolution = 0xFFFF, quint16 vResolution = 0xFFFF);
//  void addDescriptor(quint8 tag, QList<quint8> data);
};

