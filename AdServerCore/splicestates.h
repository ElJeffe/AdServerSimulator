#ifndef SPLICESTATES_H
#define SPLICESTATES_H

#include <QObject>
#include <QTimer>
#include <QDateTime>

class CChannelConfig;
class CBaseMessage;
class CCueRequestMessage;
class CAliveResponseMessage;
class CSpliceResponseMessage;
class CSpliceCompleteResponseMessage;
class CAdConfig;
class CSpliceSessions;
class CChannelLogger;
class QTime;

class CAdState: public QObject
{
  Q_OBJECT
  Q_ENUMS(AdState_t)
public:
  enum AdState_t
  {
    StateNone,
    StateInit,
    StateOnMain,
    StateOnInsertion
  };
protected:
  CChannelConfig* m_channelConfig;
  CSpliceSessions* m_spliceSessions;
  CChannelLogger* m_channelLogger;
  bool m_stateIsActive;
public:
  ~CAdState(){};
  virtual void enterState() { m_stateIsActive = true; }
  virtual void leaveState() { m_stateIsActive = false; }
  virtual bool processMessage(CBaseMessage* Message) = 0;
  virtual AdState_t getSpliceState() const = 0;
  static QString toString(AdState_t adState);
signals:
  void changeState(CAdState::AdState_t newState);
  void sendMessage(CBaseMessage* msg);
protected:
  CAdState(CChannelConfig* channelConfig, CSpliceSessions* spliceSessions, CChannelLogger* channelLogger, QObject* parent = 0);
};

class CStateNone: public CAdState
{
  Q_OBJECT
public:
  CStateNone(CChannelConfig* channelConfig, CSpliceSessions* spliceSessions, CChannelLogger* channelLogger, QObject* parent = 0):
      CAdState(channelConfig, spliceSessions, channelLogger, parent){}
  virtual AdState_t getSpliceState() const { return StateNone; }
  virtual bool processMessage(CBaseMessage* Message);
};

QDebug operator<<(QDebug dbg, CAdState::AdState_t state);

class CStateInit: public CAdState
{
  Q_OBJECT
public:
  CStateInit(CChannelConfig* channelConfig, CSpliceSessions* spliceSessions, CChannelLogger* channelLogger, QObject* parent = 0):
      CAdState(channelConfig, spliceSessions, channelLogger, parent){}
  ~CStateInit() {}
  virtual AdState_t getSpliceState() const { return StateInit; }
  virtual void enterState();
  virtual bool processMessage(CBaseMessage* Message);
};

class CStateOnMain: public CAdState
{
  Q_OBJECT
private:
  QTimer m_aliveTimer;
public:
  CStateOnMain(CChannelConfig* channelConfig, CSpliceSessions* spliceSessions, CChannelLogger* channelLogger, QObject* parent = 0);
  ~CStateOnMain() {}
  virtual AdState_t getSpliceState() const { return StateOnMain; }
  virtual bool processMessage(CBaseMessage* Message);
  virtual void enterState();
  virtual void leaveState();
private:
  bool processCueRequest(const CCueRequestMessage& cueRequest);
  bool processAliveResponse(const CAliveResponseMessage& aliveResponse);
  bool processSpliceResponse(const CSpliceResponseMessage& msg);
  bool processSpliceCompleteResponse(const CSpliceCompleteResponseMessage& msg);

  void sendSpliceRequest(const CCueRequestMessage& cueRequest);
  void sendSpliceRequest();
public slots:
  void startAd(const CAdConfig& adConfig, const QDateTime& startTime);
private slots:
  void sendAliveMessage();
};


class CStateOnInsertion: public CAdState
{
  Q_OBJECT
private:
  QTimer m_aliveTimer;
public:
  CStateOnInsertion(CChannelConfig* channelConfig, CSpliceSessions* spliceSessions, CChannelLogger* channelLogger, QObject* parent = 0);
  ~CStateOnInsertion() {}
  virtual AdState_t getSpliceState() const { return StateOnInsertion; }
  virtual bool processMessage(CBaseMessage* Message);
private:
  virtual void enterState();
  virtual void leaveState();
  bool processCueRequest(const CCueRequestMessage& cueRequest);
  bool processSpliceResponse(const CSpliceResponseMessage& msg);
  bool processSpliceCompleteResponse(const CSpliceCompleteResponseMessage& msg);
  bool processAliveResponse(const CAliveResponseMessage& msg);
public slots:
  void sendAliveMessage();
  void startB2bAd(const CAdConfig& adConfig, const QDateTime& startTime);
};
#endif // SPLICESTATES_H
