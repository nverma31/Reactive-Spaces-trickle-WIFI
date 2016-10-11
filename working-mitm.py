#!/usr/bin/env python
"""
This example builds on mitmproxy's base proxying infrastructure to
implement functionality similar to the "sticky cookies" option.

Heads Up: In the majority of cases, you want to use inline scripts.
"""
import serial
import time
import os
from mitmproxy import controller, proxy
from mitmproxy.proxy.server import ProxyServer
from mitmproxy.models import HTTPResponse

x = 0
y = 1

class StickyMaster(controller.Master):
    def __init__(self, server):
        controller.Master.__init__(self, server)
        self.stickyhosts = {}
        print 'Im here'

    def run(self):
        try:
            return controller.Master.run(self)
        except KeyboardInterrupt:
            self.shutdown()

    def handle_request(self, flow):
        global x
        global y
        hid = (flow.request.host, flow.request.port)
        if (flow.request.pretty_host.endswith("google.com") and x == 0):
            flow.request.host = "www.cmu.edu"
            ser = serial.Serial('/dev/cu.usbmodem1411', 9600)
            time.sleep(5)
            written = ser.write('5')
            ser.close()
            x =2
            print "Google:", written
        if (flow.request.pretty_host.endswith("facebook.com") and y == 1):
            flow.request.host = "www.cmu.edu"
            ser = serial.Serial('/dev/cu.usbmodem1411', 9600)
            time.sleep(5)
            written = ser.write('7')
            ser.close()
            y = 2
            print "Facebook", written

        if "cookie" in flow.request.headers:
            self.stickyhosts[hid] = flow.request.headers.get_all("cookie")
        elif hid in self.stickyhosts:
            flow.request.headers.set_all("cookie", self.stickyhosts[hid])
        flow.reply()

    def handle_response(self, flow):
        hid = (flow.request.host, flow.request.port)
        if "set-cookie" in flow.response.headers:
            self.stickyhosts[hid] = flow.response.headers.get_all("set-cookie")
        flow.reply()


config = proxy.ProxyConfig(port=8080)
server = ProxyServer(config)
m = StickyMaster(server)
m.run()
