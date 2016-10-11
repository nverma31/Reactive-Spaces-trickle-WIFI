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


"""
This example shows two ways to redirect flows to other destinations.
"""
from mitmproxy.models import HTTPResponse


def request(flow):
    # pretty_host takes the "Host" header of the request into account,
    # which is useful in transparent mode where we usually only have the IP
    # otherwise.

    # Method 1: Answer with a locally generated response
    if flow.request.pretty_host.endswith("google.com"):
        resp = HTTPResponse.make(200, b"Hello World", {"Content-Type": "text/html"})
        flow.reply.send(resp)

    # Method 2: Redirect the request to a different server
    if flow.request.pretty_host.endswith("example.org"):
        flow.request.host = "mitmproxy.org"
