//static mut X: u32 = 0;

use std::sync::atomic::AtomicI32;

static X: AtomicI32 = AtomicI32::new(0); // it has interiror mutability, so we do not need to add it as mut;
fn main() {

    let mut handles = Vec::new();

    for _ in 0..3000 {
        let handle = std::thread::spawn(|| {
            for _ in 1..100 {
                X.fetch_add(1, std::sync::atomic::Ordering::Relaxed); // second arguement is order, Relaxed means we do not care the sequence in which this happens
            }
            //unsafe {
            //   X+=1;
            //}
        });

        handles.push(handle);
    }

    handles.into_iter().for_each(|handle| {
        handle.join().unwrap();
    });


    // it won't even let be compile this unsafe code lol
    //unsafe {
     //   println!("{X}");
    //}

    println!("{}",X.load(std::sync::atomic::Ordering::Relaxed));
}
