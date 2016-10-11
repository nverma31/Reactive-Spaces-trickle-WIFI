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
        print 'running'
        hid = (flow.request.host, flow.request.port)
        a= str(hid)
        print a
        ser = serial.Serial('/dev/cu.usbmodem1421', 9600)
        time.sleep(1.5)
        written = ser.write('5')
        ser.close()
        print "Bytes Written to port:", written

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
