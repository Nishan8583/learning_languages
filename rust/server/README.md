# TODO

### Plan: 
client -> connect -> server
client -> auth -> server

Once connection is done
Server adds live user in db user1 and user2 
user 1 sends msg {"msg":"asdsad","target":"user2"}
user2 should receive it

 

 We are going to define a generic type that satisifieds certain traits, and pass this in new_app, thhis will make unit testing easier because we can use our own testdb 


 # Some questions that I had that Copilot helped me with
 Q: So if the task spanwned from tokio::spawn exits before the caller, then is the genric type instance with 'static lifetime still be there? since it has 'static lifetime

```
GitHub Copilot
Great question! The 'static lifetime bound does not mean the value will live for the entire duration of the program. It only means that the type could live that long (i.e., it does not contain non-static references).

What actually happens:

When you move a value into a tokio::spawn task, the value is owned by that task.
If the task finishes (exits) before the caller, the value is dropped at the end of the task—just like any other owned value.
The 'static bound is a compile-time guarantee that the value does not reference any stack data that could be dropped too early. It does not keep the value alive forever.
Summary:

'static is about safety, not about keeping things alive.
The value will be dropped when the task ends, even if it has a 'static bound.
So, if your spawned task exits before the caller, the generic type instance will be dropped at that point, just like normal Rust ownership rules. 'static only ensures it is safe to move into the task.
```