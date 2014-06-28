import json
import time
import hashlib

user = 'testuser'
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
