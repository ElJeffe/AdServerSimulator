import sys
from time import sleep
from PyQt4 import QtGui
from PyQt4 import QtCore
from AdServerCore import *
import pdb

class test:
  def start(self):
    a = QtCore.QCoreApplication(sys.argv)
    
    self.adChannel =CAdChannel()
    
    channelConfig = CChannelConfig()
    channelConfig.m_splicerAddress = "10.50.203.58"
    channelConfig.m_channelName = "test"
    channelConfig.m_card = 4
    channelConfig.m_port = 1
    
    adBlock = CAdBlock()
    self.adChannel.stateChanged.connect(self.stateChanged)
    self.adChannel.setChannelConfig(channelConfig)
    self.adChannel.connectSplicer()


    
    a.exec_()

  @QtCore.pyqtSlot(CAdState.AdState_t)
  def stateChanged(self, adState):
    print "new state %d" % self.adChannel.state() 
    if (adState == CAdState.StateNone):
      print("StateNone")
    elif (adState == CAdState.StateInit):
      print("StateInit")
    elif (adState == CAdState.StateOnMain):
      print("OnMain")
    elif (adState == CAdState.StateOnInsertion):
      print("OnInsertion")

app = test()
app.start()
