import json
import cocaine
import getpass
import hashlib

def help(**args):
    print """Support tool for cocaine auth plugin.
help  this help
list  list objects"""

def check(auth, storage, ns, args):
    user = args.options.pop(0)
    pwd = getpass.getpass('Password: ')
    auth_type = 'plain'
    if 'plain' not in args.options:
        pwd = hashlib.md5(pwd).hexdigest()
        auth_type = 'md5'
    else:
        args.options.remove('plain')
    r = auth.authenticate(auth_type, user, pwd).get()
    if (r[0] == False):
        print 'Authentication of {0} failed: {1}'.format(user, r[1])
        return
    print '{0} authenticated successfully'.format(user)
    token = r[1]
    for perm in args.options:
        r = auth.authorize(token, perm).get()
        if (r[0] == True):
            print '{0} is permitted to {1}'.format(user, perm)
        else:
            print '{0} is not permitted to {1}'.format(user, perm)
    r = auth.logout(token).get()
    if (r[0] == True):
        print '{0} logged out successfully'.format(user)
    else:
        print 'Error while {0} logout: {1}'.format(user, r[1])

def list(auth, storage, ns, args):
    if args.realm is not None: ns += args.realm
    ns = ns.replace('/', '.')
    if 'permissions' in args.options:
        print storage.read(ns, 'permissions.json').get()
    try:
        index = json.load(storage.read(ns, 'index.json').get())
    except cocaine.exceptions.ServiceError as e:
        print "Auth storage is empty"
        return
    try:
        None
    except cocaine.exceptions.ServiceError as e:
        print "Error in cocaine service {0}:".format(e.servicename)
        print e.msg

def list_permissions(auth, storage, ns, args):
    None
