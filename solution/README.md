The "Message" and "User" struct have the same format; and when a user disconnect, the User object is freed but the pointer is not deleted.
When you send the next message, it gets allocated there instead; overwriting the "is_admin" flag with the "length" of the message.


--> create user "hugo"
--> create user "bob"
--> disconnect "hugo" (ctrl + c)
--> bob writes "hugo" --> gets allocated on heap instead of users
--> hugo comes back with nickname hugo
--> he is admin
--> /flag

Wait for all other users to stop talking makes it easier

