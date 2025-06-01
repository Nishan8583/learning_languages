fn main() {
    const N_THREADS: usize = 8;  // we will divide the data into 8 chunks;

    let data: Vec<u32> = (0..10000).collect();

    let mut thread_handles = Vec::new();

    let chunks = data.chunks(N_THREADS);  // chunks() dividdes the Vector into chunks of size N_THREADS

    for chunk in chunks {
        let my_chunk = chunk.to_owned(); // since the thread is going to take ownership of it, we need to clone it, to_owned either takes ownerhsip of it or simply does a clone()

        let handle = std::thread::spawn(move || {
            my_chunk.iter().sum::<u32>()
        });

        thread_handles.push(handle);
    }


    let mut sum = 0;
    for handle in thread_handles {
        sum += handle.join().unwrap(); // join() waits for the thread to finish and returns a Result, we use unwrap to get the value from the Result
    }

    println!("Total sum is: {}",sum);
}
