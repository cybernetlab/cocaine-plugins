#!/usr/bin/env python

import json
import time
import hashlib
import argparse
import sys
import os.path
from cocaine.asio.service import Service

def find_service(config, type):
    if 'services' not in config: return None
    result = {}
    for k, v in config['services'].iteritems():
        if 'type' in v and v['type'] == type: result[k] = v
    return result;

def find_storage(config, name):
    services = find_service(config, 'storage')
    for k, v in services.iteritems():
        if 'args' in v and 'backend' in v['args'] and v['args']['backend'] == name:
            return k
    return None

parser = argparse.ArgumentParser(description = 'Support tool for cocaine auth plugin.')
parser.add_argument('-c', '--config', dest = 'config',
                    help = 'cocaine config file (default: None)')
parser.add_argument('-a', '--auth', dest = 'auth', default = 'auth',
                    help = 'auth service name (default: auth)')
parser.add_argument('-s', '--storage', dest = 'storage',
                    help = 'storage service name (default: from config file or `storage`)')
parser.add_argument('-n', '--ns', dest = 'ns',
                    help = 'auth namespace (default: from config file or storage root)')
parser.add_argument('-r', '--realm', dest = 'realm',
                    help = 'auth realm (default: root realm)')
parser.add_argument('command', metavar = 'COMMAND',
                    help = 'command to execute')
parser.add_argument('options', metavar = 'OPTS', nargs = '*', default = [],
                    help = 'command options')
args = parser.parse_args()

import commands
command = getattr(commands, args.command, None)
if command is None:
    sys.exit("Wrong command {0}. Use `{1} help` to list commands".format(
        args.command, sys.argv[0]
    ))

if args.config is not None and os.path.isfile(args.config):
    config = json.load(open(args.config))
    auths = find_service(config, 'auth')
    if len(auths) == 0:
        sys.exit('Cocaine config has no auth services');
    if args.auth not in auths:
        sys.exit('Cocaine config has no auth service {0}'.format(args.auth));
    auth = Service(args.auth)
    storage = Service(find_storage(config, auths[args.auth]['args']['storage']['name']))
    ns = auths[args.auth]['args']['storage']['namespace']
else:
    auth = Service(args.auth)
    storage = Service('storage' if args.storage is None else args.storage)
    ns = '' if args.ns is None else args.ns

command(auth = auth, storage = storage, ns = ns, args = args)


'''
user = 'testuser'
subuser = 'user1@realm1'
password = 'nopass'
perm1 = 'view_document'
perm2 = 'create_document'

password_md5 = hashlib.md5(password).hexdigest()

from cocaine.asio.service import Service

auth = Service('auth')

print('Test authenication')
print('  md5 result:', auth.authenticate('md5', user, password_md5).get())
print('  plain result:', auth.authenticate('plain', user, password).get())
print('')

result = auth.authenticate('plain', user, password).get()
if (result[0] == True):
    print('Test authorize')
    print('  authorize for perm1 result:', auth.authorize(result[1], perm1).get())
    print('  authorize for perm2 result:', auth.authorize(result[1], perm2).get())
    print('')
    print('Test logout')
    print('  logout result:', auth.logout(result[1]).get())
    print('')

print('Test realm\'ed authenication')
print('  md5 result:', auth.authenticate('md5', subuser, password_md5).get())
print('  plain result:', auth.authenticate('plain', subuser, password).get())
print('')

result = auth.authenticate('md5', subuser, password_md5).get()
if (result[0] == True):
    print('Test authorize')
    print('  authorize for perm1 result:', auth.authorize(result[1], perm1).get())
    print('  authorize for perm2 result:', auth.authorize(result[1], perm2).get())
    print('')
    print('Test logout')
    print('  logout result:', auth.logout(result[1]).get())
    print('')
'''
