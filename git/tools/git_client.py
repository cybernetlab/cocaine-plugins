from cocaine.services import Service

git = Service('git')
print git.create('test').get()
