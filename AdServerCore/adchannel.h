#ifndef ADCHANNEL_H
#define ADCHANNEL_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QIODevice>
#include <QMap>
#include "splicestates.h"
#include "channelconfig.h"
#include "channellogger.h"

class QTcpSocket;
class CSpliceSessions;


class CAdChannel : public QObject
{
  Q_OBJECT

private:
  CChannelConfig m_channelConfig;
  CSpliceSessions* m_spliceSessions;
  CChannelLogger* m_channelLogger;
  QTcpSocket* m_socket;
  CAdState::AdState_t m_currentState;
  QByteArray m_unprocessedData;
  typedef QMap< CAdState::AdState_t, CAdState* > SpliceStatesMap_t;
  SpliceStatesMap_t m_spliceStates;
public:
  explicit CAdChannel(QObject *parent = 0);
  ~CAdChannel();
  void setChannelConfig(const CChannelConfig& channelConfig);
  const CChannelConfig& getChannelConfig() const { return m_channelConfig; }
  CChannelLogger* getChannelLogger() const { return m_channelLogger; }
  const CSpliceSessions* getSpliceSessions() const { return m_spliceSessions; }
  CAdState::AdState_t state() const { return m_currentState; }
  void saveLog(const QString& fileName) const;
  bool loadLog(const QString& fileName);


  bool connectSplicer();
  bool disconnectSplicer();

private:
  bool fullMessageAvailable(QIODevice& ioDevice) const;
  void parseMessage(QIODevice& ioDevice) const;

signals:
  void stateChanged(CAdState::AdState_t newState);

public slots:
  void changeState(CAdState::AdState_t newState);
  void sendMessage(CBaseMessage* msg);

private slots:
  void readData();
  void socketDisconnected();

};

#endif // ADCHANNEL_H
