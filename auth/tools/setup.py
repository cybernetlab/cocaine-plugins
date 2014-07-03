#!/usr/bin/env python

import json
import hashlib
import argparse
import sys
import getpass
import cocaine

from cocaine.asio.service import Service
from cocaine.exceptions import ServiceError

parser = argparse.ArgumentParser(description = 'Setup tool for cocaine auth plugin.')
parser.add_argument('-t', '--storage-type', dest = 'storage_type',
                    default = 'cocaine_storage',
                    help = 'auth storage type (default: cocaine_storage)')
parser.add_argument('-s', '--storage', dest = 'storage', default = 'storage',
                    help = 'auth storage name (default: storage)')
parser.add_argument('-n', '--namespace', dest = 'ns', default = 'authdb',
                    help = 'auth root folder in storage (default: authdb)')
args = parser.parse_args()

if args.storage_type not in ['cocaine_storage']:
    sys.exit('Wrong auth storage type {0}'.format(args.storage_type))

user = raw_input('Enter superadmin user name (default: admin): ')
if not user: user = 'admin'

pwd = ''
while not pwd:
    pwd = getpass.getpass('Enter superadmin password: ')

confirm = ''
while pwd != confirm:
    confirm = getpass.getpass('Confirm password: ')

admin = {
    'name': user,
    'type': 'user',
    'password': hashlib.md5(pwd).hexdigest(),
    'roles': [ 'superadmin' ]
}

permissions = [
    { 'allow': [ 'superadmin' ], 'to': [ '*' ] }
]

index = {
    'users': [ 'users/{0}'.format(user) ]
}

config = {
    'authentication': [ 'md5' ]
}

if args.storage_type == 'cocaine_storage':
    try:
        storage = Service(args.storage)
        storage.write(
            "{0}/users".format(args.ns),
            user + '.json',
            json.dumps(admin, indent = 4),
            []
        )
        storage.write(args.ns, 'index.json', json.dumps(index, indent = 4), [])
        storage.write(args.ns, 'permissions.json', json.dumps(permissions, indent = 4), [])
        storage.write(args.ns, 'config.json', json.dumps(config, indent = 4), [])
        storage.read(args.ns, 'config.json').get()
    except ServiceError as e:
        print 'Error in cocaine service {0}:'.format(e.servicename)
        print e.msg

print 'Auth storage initialized successfully'
