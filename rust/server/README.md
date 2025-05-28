# TODO

### Plan: 
client -> connect -> server
client -> auth -> server

Once connection is done
Server adds live user in db user1 and user2 
user 1 sends msg {"msg":"asdsad","target":"user2"}
user2 should receive it

 

 We are going to define a generic type that satisifieds certain traits, and pass this in new_app, thhis will make unit testing easier because we can use our own testdb 