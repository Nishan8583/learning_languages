# Pinnning

### ✅ Why do we pin in Rust?

Pinning is used in async and other systems programming contexts to guarantee that a value will not be moved in memory after it has been pinned. This matters because some types (especially futures and self-referential structs) rely on their memory addresses remaining fixed.

### 💡 Answer to Your Main Question:
```
    So why do we pin? Is it because when we await we can not know when the task in async is going to be executed? Or is it because Rust can not guarantee the ownership of the returned value from async during runtime?
```
### 🟩 The correct reasoning is:
```
    ✅ Pinning is needed because Rust cannot guarantee that a Future will not be moved once it is being polled. And many Futures contain self-references or internal state that assumes a fixed memory location.
```

### 🔍 More details:
1. async functions return Futures

    When you write an async fn, the compiler generates a state machine behind the scenes.

    That state machine can contain self-references (e.g., pointers to internal fields).

    If it gets moved after the first .await, those self-references may point to invalid memory — that's unsafe.

2. The Future trait requires a Pin<&mut Self>

    The poll() function has this signature:

    fn poll(self: Pin<&mut Self>, cx: &mut Context<'_>) -> Poll<Self::Output>;

    This means: before polling (aka driving the future forward), you must guarantee that the future won’t be moved.


### 🧠 So when do we need to pin?

    ✅ When manually polling a future (e.g. writing your own executor).

    ✅ When using tokio::select!, because futures must be polled without moving.

    ✅ When returning trait objects like dyn Future — these need to be heap allocated and pinned (Pin<Box<dyn Future>>) so that the executor can poll them safely.

    ❌ You don’t need to pin if you're simply .awaiting a value returned from an async fn — Rust handles the pinning internally.

### 🧪 Your Code Notes: Let's Review One Important Part

```rust
let mut future = async {
    println!("hello from future");
};

tokio::pin!(future); // macro expands to: let mut future = Pin::new(&mut future);
(&mut future).await; // now it's safe to await because it's pinned
```

This pattern is needed because tokio::select! and similar constructs poll multiple futures, and Rust needs to guarantee they don't move during polling.

### 📦 TL;DR Summary

    ✅ We pin to guarantee that a future won't be moved in memory after polling begins.

    ❗ This is required for safety when:

        1. The future may contain self-references.

        2. The future will be stored somewhere and polled later.

        3. The future is trait object (dyn Future), which is always unsized and behind a pointer (Box or &).

    ❌ It is not because we don't know when the async task executes or due to ownership issues specifically — it's about memory safety guarantees related to self-references and immovable types.