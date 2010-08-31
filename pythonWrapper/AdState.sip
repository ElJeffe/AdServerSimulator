// define the SIP wrapper for a SpliceState

%ModuleCode
#include "splicestates.h"
%End

%Import QtCore/QtCoremod.sip


class CAdState: QObject
{
%TypeHeaderCode
#include "splicestates.h"
%End

public:
  enum AdState_t
  {
    StateNone,
    StateInit,
    StateOnMain,
    StateOnInsertion
  };

  static QString toString(AdState_t adState);

private:
  CAdState();
  ~CAdState();
};

