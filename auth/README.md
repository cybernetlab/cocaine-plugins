## Auth sevice for cocaine

> **WARNING !** this plugin is in active development stage!

Cocaine plugin for authentication and authorization.

Now only plain and md5-encoded passwords authentication and simple role-based authorization are supported. All auth data can be stored on any cocaine storage.

### Build

```sh
cd cocaine-plugins
git submodule update --init
sudo apt-get install equivs devscripts
# boost regex required for auth plugin
sudo apt-get install libboost-regex-dev
sudo mk-build-deps -ir
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/usr -DAUTH=ON ..
make
sudo make install
```

to build only auth plugin use following `cmake` command instead above:

```sh
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/usr -DAUTH=ON -DBLASTBEAT=OFF -DCACHE=OFF -DDOCKER=OFF -DIPVS=OFF -DLOGSTASH=OFF -DMONGO=OFF -DPYTHON=OFF -DCHRONO=OFF -DURLFETCH=OFF -DZEROMQ=OFF -DELASTICSEARCH=OFF ..
```

### Configuration

Sample cocaine config part regarding auth (remove comments when copy-paste):

```json
{
    "services": {
        "auth": {
            "type": "auth", // required
            "args": {
                // required
                "storage": {
                    // required, only `cocaine_storage` supported now
                    "type": "cocaine_storage",
                    // required, storage name
                    "name": "core",
                    // optional, prefix all storage operations with this value
                    "namespace": "authdb"
                },
                // optional
                "cache": {
                    "type": "memory" // or can be `cocaine_storage`
                }
            }
        }
    },
}
```

With above config following file structure assumed:

```
storage_root
  authdb
    subdomain
      users
        other-user-1.json
        other-user-2.json
      config.json
      permissions.json
    users
      user1.json
      user2.json
    config.json
    permissions.json
    index.json
    sessions.json
```

You can use `tools/setup.py` utility to make initial layout of auth database (make sure to install [cocaine python framework](https://github.com/cocaine/cocaine-framework-python) before use this utility).

You can have nested authentication domains with separate configuration, permissions and users. Also you can control child domain permissions in parent `permission.json`.

#### `config.json`:

Now you can only select allowed authentication methods list.

```json
{
    // allowed authentication methods
    "authentication": [ "md5", "plain" ]
}
```

#### `user_name.json`:

```json
{
    // required. should be same as file name (without .json)
    "name" : "user_name",
    // required. should be exactly `user`
    "type" : "user",
    // required. passwords are stored in md5-encoded form
    "password" : "0945fc9611f55fd0e183fb8b044f1afe",
    // should be an array
    "roles" : [ "guest" ],
    // if false then user can take many sessions
    "singleSession" : true,
    // all opened sessions of this user
    // hands off! Maintained automatically
    "sessions": []
}
```

> **NOTE** When adding users manually dont't forget to add it into root `index.json` file.

#### `permissions.json`:

You can allow or deny any role to (from) some permissions. Permissions usually splitted by `:` and denotes subject of permission and the verb.

Single `*` without any other characters matches any role or permission.

`*` as placeholder in roles and permissions names matches any character except `.` and `:` for permissions or `@` for roles. For example, `*:create` will match `document:create` but will not match `pdf.document:create`. Same for roles: `guest@*` will match `guest@subdomain` but will not match `guest` and `guest@more.levels.domain`.

`**` matches any character except `:` for permission or `@` for roles. For example, `**:create` will match `document:create` and `pdf.document:create`. For roles: `guest@**` will match `guest` user in any subdomain.

```json
// should be an array
[
    {
        // allow or deny required and should be an array of roles
        "allow": [ "guest" ],
        // `to` is required and should be an array of permission names
        "to": [ "document:view", "document:list" ]
    },

    {
        "deny": [ "guest" ],
        "to": [ "*:create" ]
    },

    {
        "deny": [ "guset@subdomain" ],
        "to": [ "document:view" ]
    },

    {
        "deny": [ "guest@**" ],
        "to": [ "**:create" ]
    }
]
```

#### `index.json`:

```json
{
    users: [
        "users/user1",
        "users/user2",
        "subdomain/users/other-user-1",
        "subdomain/users/other-user-2",
    ]
}
```

#### `sessions.json`

This file maintained automatically and should not be touched by users. Contains all opened sessions.

### API

#### [result, token] authenticate(string method, string name, string data)

Authenticates user with `name` and authentication `data` (in many cases it is a password). Returns array with _boolean_ `result` and _string_ `token` (other name is `session`) that can be used later to authorize user. If `result` is `false`, second element contains `reason` of failed authentication.

if `name` doesn't contain `@` char, it's assumed as user name in root realm. If `@` specified in the `name`, it separates user name (before `@`) and realm name (after `@`).

#### [result, reason] logout(string token)

Removes user session. Returns array with _boolean_ `result` and _string_ `reason` for fails.

#### [result, ticket] authorize(string token, string perm)

Authorizes allready authenticated user to specified permission `perm`. Returns array with _boolean_ `result` and _string_ `reason` for fails. `ticket` is not supported yet and empty string returned on success.

### Changes

`11.1.1`:

* `authentication` config option moved to local `config.json` files
* `cache` main config option added
* `memory` storage added for caching
* nested realms
* nested permissions
* regex searching of roles and permissions

`11.1.0`:

* `namespace` option moved into storage section

### TODO:

* More authentication methods
* In progress: Rich permissions system
* Authorization tickets
* In progress: Cache auth system locally
* Tokens and tickets TTL
