// define the SIP wrapper for a Descriptor

%ModuleCode
#include "scte30messages.h"
%End

%Import QtCore/QtCoremod.sip


class CDescriptor
{
%TypeHeaderCode
#include "scte30messages.h"
%End

public:
  quint8 m_tag;
public:
  CDescriptor();
};

