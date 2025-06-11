// Modren OS scheduler are pretty awesome, so setting core affinity by ourselves can degrade performance
// Be sure to benchmark first
fn main() {
    // setting core affinity can be platform specific, so use this crate
    let core_ids = core_affinity::get_core_ids().unwrap();


    let handles = core_ids.into_iter().map(|id| {
        std::thread::spawn(move || {
            // pinning to CPU 0 can fail
            // Depends on the OS
            let success = core_affinity::set_for_current(id);
            if success {
                println!("Success for {id:?}")
            } else {
            println!("{success}");
            }
        })
    }).collect::<Vec<_>>();

    handles.into_iter().for_each(|h| {
        h.join().unwrap();
    });
}
