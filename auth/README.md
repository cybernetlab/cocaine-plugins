## Auth sevice for cocaine

> **WARNING !** this plugin is in active development stage!

Cocaine plugin for authentication and authorization.

Now only plain and md5-encoded passwords authentication and simple role-based authorization are supported. All auth data can be stored on any cocaine storage.

### Configuration

Sample cocaine config part regarding auth (remove comments when copy-paste):

```json
{
    "services": {
        "auth": {
            "type": "auth", // required
            "args": {
                "storage": {                   // required
                    "type": "cocaine_storage", // only this supported now
                    "name": "core"             // storage name
                },
                // prefix all storage operations with this value
                "namespace": "authdb",
                // allowed authentication methods
                "authentication": [ "md5", "plain" ]
            }
        }

    },
}
```

With above config following file structure assumed:

```
storage_root
  authdb
    users
      user1.json
      user2.json
    sessions.json
    permissions.json
```

`user_name.json`:

```json
{
    // required. should be same as file name (without .json)
    "name" : "user_name",
    // required. should be exactly `users`
    "type" : "users",
    // passwords are stored in md5-encoded form
    "password" : "0945fc9611f55fd0e183fb8b044f1afe",
    // should be an array
    "roles" : [ "guest" ]
}
```

`permissions.json`:

```json
// should be an array
[
    {
        // allow or deny required and should be an array of roles
        "allow": [ "guest" ],
        // `to` is required and should be an array of permission names
        "to": [ "view_document" ]
    },

    {
        "deny": [ "guest" ],
        "to": [ "create_document" ]
    }
]
```

`sessions.json` is maintained automatically and should not be touched by users.

### API

#### [result, token] authenticate(string method, string name, string data)

Authenticates user with `name` and authentication `data` (in many cases it is a password). Returns array with _boolean_ `result` and _string_ `token` that can be used to authenticate user. If `result` is `false`, second element contains `reason` of failed authentication.

#### [result, reason] logout(string token)

Removes user session. Returns array with _boolean_ `result` and _string_ `reason` for fails.

#### [result, ticket] authorize(string token, string perm)

Authorizes allready authenticated user to specified permission `perm`. Returns array with _boolean_ `result` and _string_ `reason` for fails. `tokens` is not supported yet and empty string returned on success.

### TODO:

* More authentication methods
* Rich permissions system
* Authorization tickets
* Cache auth system locally
* Tokens and tickets TTL
