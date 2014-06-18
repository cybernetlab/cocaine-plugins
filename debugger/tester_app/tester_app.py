import asyncio
import logging
import sys

from tornado.ioloop import IOLoop
# from cocaine import concurrent
from cocaine.services import Service

#@concurrent.engine
@asyncio.coroutine
def start():
    log = logging.getLogger()
    log.debug('calling `start` method')
    try:
        response = yield debugger.start('service_name')
        log.debug('response received')
        log.debug(response)
    except Exception as err:
        log.debug('error occured')
        log.debug(repr(err))
        # print(repr(err))
    finally:
        # loop.stop()
        IOLoop.current().stop()

if __name__ == '__main__':
    log = logging.getLogger()
    console = logging.StreamHandler(sys.stdout)
    console.setLevel(logging.DEBUG)
    log.addHandler(console)
    log.setLevel(logging.DEBUG)

    log.debug('starting tester application')
#    print("Starting tester application")
    debugger = Service('debugger')
    #log.debug('calling start()')
    #start()
    #log.debug('start() called')
    #IOLoop.configure('tornado.platform.asyncio.AsyncIOLoop')
    #IOLoop.instance().run_sync(start)
    #IOLoop.instance().start()
    loop = asyncio.get_event_loop()
    loop.run_until_complete(start())

    console.flush()
    console.close()
    log.removeHandler(console)
    print('Done')
